#if(BUILD_TYPE == GPU)

#include "primitives.h"

//*********************************************************************//
//Device forms

//F_call
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_call_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j, const int sID)
{
	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Call);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Call);
	for(int index=0; index < entry_count_host[STREAM_CALL]; ++index)
	{
		column_select(Fun, s_indices[sID], index, Fun_vec[sID], stream_Call);
		ell_spmv(sigma, Fun_vec[sID], vf[sID], stream_Call);

		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Call);

		for(int i=0; i<j; ++i)
		{
			column_select(Arg[i], s_indices[sID], index, Arg_vec[sID], stream_Call);
			ell_spmv(sigma, Arg_vec[sID], v[i], stream_Call);
			csr_spmv(Var[i], vf[sID], a[i], stream_Call);
		}
		
		for(int i=0; i<j; ++i)
		{
			gather_reduce(v[i], temp_row_indices[sID], index_count[sID], 0, stream_Call);
			gather_reduce(a[i], temp_col_indices[sID], index_count[sID], 1, stream_Call);
			OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], sigma, stream_Call);
		}
	}

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Call);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Call);
}

//f_call
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_call()
{
	for(int j=1; j<=m_maxCall; ++j)
	{
		if(valid_Call[j])
		{
			cudaEvent_t count_entries;
			checkCudaErrors( cudaEventCreateWithFlags(&count_entries, 0) );

			AND_OP<VALUE_TYPE> (r_prime, Call[j], s[STREAM_CALL], stream_Call);
			count(s[STREAM_CALL], 1, &entry_count_host[STREAM_CALL], &entry_count_device[STREAM_CALL], stream_Call);
			checkCudaErrors(cudaEventRecord(count_entries, stream_List));
			checkCudaErrors(cudaStreamWaitEvent(stream_List, count_entries, 0));

			fprintf(stderr, "f_call_%d: %d\n", j, entry_count_host[STREAM_CALL]);			
			if(entry_count_host[STREAM_CALL] > 0)
				f_call_device(s[STREAM_CALL], j, STREAM_CALL);
		}
	}
}

//F_list
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_list_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j, const int sID)
{
	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_List);
	
	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_List);
	for(int i=0; i < entry_count_host[STREAM_LIST]; ++i)
	{
		FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_List);
		FILL<VALUE_TYPE> (v_list, 0, stream_List);

		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_List);
		ell_spmv(sigma, Fun_vec[sID], vf[sID], stream_List);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_List);

		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_List);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_List);

		for(int k=0; k<j; ++k)
		{
			column_select(Arg[k], s_indices[sID], i, Arg_vec[sID], stream_List);
			ell_spmv(sigma, Arg_vec[sID], v[k], stream_List);
			AccumVec<VALUE_TYPE> (v_list, v[k], stream_List);
		}
		AccumVec<VALUE_TYPE> (v_list, LIST_vec, stream_List);

		gather_reduce(v_list, temp_row_indices[sID], index_count[sID], 0, stream_List);
		gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_List);
		OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], sigma, stream_List);
	}

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_List);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_List);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_list()
{
	for(int j=0; j<=m_maxList; ++j)
	{
		if(valid_List[j])
		{
			cudaEvent_t count_entries;
			checkCudaErrors( cudaEventCreateWithFlags(&count_entries, 0) );

			AND_OP<VALUE_TYPE> (r_prime, PrimList[j], s[STREAM_LIST], stream_List);
			count(s[STREAM_LIST], 1, &entry_count_host[STREAM_LIST], &entry_count_device[STREAM_LIST], stream_List);
			checkCudaErrors(cudaEventRecord(count_entries, stream_List));
			checkCudaErrors(cudaStreamWaitEvent(stream_List, count_entries, 0));

			fprintf(stderr, "f_list_%d: %d\n", j, entry_count_host[STREAM_LIST]);
			if(entry_count_host[STREAM_LIST] > 0)	
				f_list_device(s[STREAM_LIST], j, STREAM_LIST);
		}
	}
}

// //F_set
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_set_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Set);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Set);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Set);
	//temp_Mat[sID*4 + 0] = sigma;
	for(int i=0; i < entry_count_host[STREAM_SET]; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Set);
		ell_spmv(sigma, Fun_vec[sID], vf[sID], stream_Set);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Set);
		column_select(Arg[0], s_indices[sID], i, a_set, stream_Set);
		column_select(Arg[1], s_indices[sID], i, Arg_vec[sID], stream_Set);
		ell_spmv(sigma, Arg_vec[sID], v_set, stream_Set);

		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Set);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Set);
		
		gather_reduce(v_set, temp_row_indices[sID], index_count[sID], 0, stream_Set);
		gather_reduce(a_set, temp_col_indices[sID], index_count[sID], 1, stream_Set);
		OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], sigma, stream_Set);		
	}

	//sigma + (a_var (X) void) + (a_set (X) v_set)
	gather_reduce(VOID_vec, temp_row_indices[sID], index_count[sID], 0, stream_Set);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Set);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], sigma, stream_Set);

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Set);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Set);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_set()
{
	cudaEvent_t count_entries;
	checkCudaErrors( cudaEventCreateWithFlags(&count_entries, 0) );

	AND_OP<VALUE_TYPE> (r_prime, SET, s[STREAM_SET], stream_Set);
	count(s[STREAM_SET], 1, &entry_count_host[STREAM_SET], &entry_count_device[STREAM_SET], stream_Set);
	checkCudaErrors(cudaEventRecord(count_entries, stream_Set));
	checkCudaErrors(cudaStreamWaitEvent(stream_Set, count_entries, 0));

	fprintf(stderr, "f_set: %d\n", entry_count_host[STREAM_SET]);	
	if(entry_count_host[STREAM_SET] > 0)
		f_set_device(s[STREAM_SET], STREAM_SET);
}

//F_if
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_if_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_IF);
	for(int i=0; i < entry_count_host[STREAM_IF]; ++i)
	{
		column_select(Arg[0], s_indices[sID], i, Arg_vec[sID], stream_IF);
		ell_spmv(sigma, Arg_vec[sID], v_cond, stream_IF);
	
		InnerProductStore<VALUE_TYPE> (NOT_FALSE_vec, v_cond, AND_vec1, i, stream_IF);
		InnerProductStore<VALUE_TYPE> (FALSE_vec, v_cond, AND_vec2, i, stream_IF);

		column_select_if<INDEX_TYPE, VALUE_TYPE> (CondTrue, s_indices[sID], AND_vec1, i, Cond_vec, stream_IF);
		AccumVec<VALUE_TYPE> (r_prime, Cond_vec, stream_IF);
		column_select_if<INDEX_TYPE, VALUE_TYPE> (CondFalse, s_indices[sID], AND_vec2, i, Cond_vec, stream_IF);
		AccumVec<VALUE_TYPE> (r_prime, Cond_vec, stream_IF);
	}
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_if()
{
	cudaEvent_t count_entries;
	checkCudaErrors( cudaEventCreateWithFlags(&count_entries, 0) );

	AND_OP<VALUE_TYPE> (r_prime, IF, s[STREAM_IF], stream_IF);
	count(s[STREAM_IF], 1, &entry_count_host[STREAM_IF], &entry_count_device[STREAM_IF], stream_IF);
	checkCudaErrors(cudaEventRecord(count_entries, stream_IF));
	checkCudaErrors(cudaStreamWaitEvent(stream_IF, count_entries, 0));

	fprintf(stderr, "f_if: %d\n", entry_count_host[STREAM_IF]);
	if(entry_count_host[STREAM_IF] > 0)
		f_if_device(s[STREAM_IF], STREAM_IF);
}

//F_primNum
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primNum_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Num);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Num);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Num);
	for(int i=0; i < entry_count_host[STREAM_NUM]; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Num);
		ell_spmv(sigma, Fun_vec[sID], vf[sID], stream_Num);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Num);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Num);
		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Num);
	}

	//sigma + (a_var (X) NUM)
	gather_reduce(NUM_vec, temp_row_indices[sID], index_count[sID], 0, stream_Num);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Num);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], sigma, stream_Num);

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Num);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Num);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primNum()
{
	cudaEvent_t count_entries;
	checkCudaErrors( cudaEventCreateWithFlags(&count_entries, 0) );

	AND_OP<VALUE_TYPE> (r_prime, PrimNum, s[STREAM_NUM], stream_Num);
	count(s[STREAM_NUM], 1, &entry_count_host[STREAM_NUM], &entry_count_device[STREAM_NUM], stream_Num);
	checkCudaErrors(cudaEventRecord(count_entries, stream_Num));
	checkCudaErrors(cudaStreamWaitEvent(stream_Num, count_entries, 0));

	fprintf(stderr, "f_primNum: %d\n", entry_count_host[STREAM_NUM]);
	if(entry_count_host[STREAM_NUM] > 0)
		f_primNum_device(s[STREAM_NUM], STREAM_NUM);
}

//F_primBool
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primBool_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Bool);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Bool);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Bool);
	for(int i=0; i < entry_count_host[STREAM_BOOL]; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Bool);
		ell_spmv(sigma, Fun_vec[sID], vf[sID], stream_Bool);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Bool);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Bool);
		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Bool);
	}

	//sigma + (a_var (X) #T#F)
	gather_reduce(BOOL_vec, temp_row_indices[sID], index_count[sID], 0, stream_Bool);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Bool);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], sigma, stream_Bool);

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Bool);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Bool);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primBool()
{
	cudaEvent_t count_entries;
	checkCudaErrors( cudaEventCreateWithFlags(&count_entries, 0) );

	AND_OP<VALUE_TYPE> (r_prime, PrimBool, s[STREAM_BOOL], stream_Bool);
	count(s[STREAM_BOOL], 1, &entry_count_host[STREAM_BOOL], &entry_count_device[STREAM_BOOL], stream_Bool);
	checkCudaErrors(cudaEventRecord(count_entries, stream_Bool));
	checkCudaErrors(cudaStreamWaitEvent(stream_Bool, count_entries, 0));

	fprintf(stderr, "f_primBool: %d\n", entry_count_host[STREAM_BOOL]);
	if(entry_count_host[STREAM_BOOL] > 0)
		f_primBool_device(s[STREAM_BOOL], STREAM_BOOL);
}

//F_primVoid
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primVoid_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Void);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Void);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Void);
	for(int i=0; i < entry_count_host[STREAM_VOID]; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Void);
		ell_spmv(sigma, Fun_vec[sID], vf[sID], stream_Void);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Void);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Void);
		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Void);
	}

	//sigma + (a_var (X) VOID)
	gather_reduce(VOID_vec, temp_row_indices[sID], index_count[sID], 0, stream_Void);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Void);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], sigma, stream_Void);

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Void);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Void);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primVoid()
{
	cudaEvent_t count_entries;
	checkCudaErrors( cudaEventCreateWithFlags(&count_entries, 0) );
	
	AND_OP<VALUE_TYPE> (r_prime, PrimVoid, s[STREAM_VOID], stream_Void);
	DEBUG_PRINT("s[STREAM_VOID]: ", s[STREAM_VOID]);
	count(s[STREAM_VOID], 1, &entry_count_host[STREAM_VOID], &entry_count_device[STREAM_VOID], stream_Void);
	checkCudaErrors(cudaEventRecord(count_entries, stream_Void));
	checkCudaErrors(cudaStreamWaitEvent(stream_Void, count_entries, 0));

	fprintf(stderr, "f_PrimVoid: %d\n", entry_count_host[STREAM_VOID]);
	if(entry_count_host[STREAM_VOID] > 0)
		f_primVoid_device(s[STREAM_VOID], STREAM_VOID);
}

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::LoadEllMatrix(
			cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src,
			cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &dst)
{
	LoadEllMatrix_device(src, dst);
}

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::LoadEllMatrix(
			cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src,
			shared_store &dst)
{
	LoadEllMatrix_device(src, dst);
}

#endif