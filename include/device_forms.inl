#if(BUILD_TYPE == GPU)

#include "primitives.h"

//*********************************************************************//
//Device forms

//F_call
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_call_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j, const int sID)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	fprintf(stderr, "call: %d\n", entry_count);
	if(entry_count == 0)
		return;

	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Call);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Call);
	for(int index=0; index < entry_count; ++index)
	{
		column_select(Fun, s_indices[sID], index, Fun_vec[sID], stream_Call);
		ell_spmv(shared_sigma, Fun_vec[sID], vf[sID], stream_Call);

		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Call);

		for(int i=0; i<j; ++i)
		{
			column_select(Arg[i], s_indices[sID], index, Arg_vec[sID], stream_Call);
			ell_spmv(shared_sigma, Arg_vec[sID], v[i], stream_Call);
			csr_spmv(Var[i], vf[sID], a[i], stream_Call);
		}
		
		//temp_Mat[sID*4 + 0] = shared_sigma;
		for(int i=0; i<j; ++i)
		{
			gather_reduce(v[i], v_indices, index_count[sID], 0, stream_Call);
			gather_reduce(a[i], a_indices, index_count[sID], 1, stream_Call);
			OuterProductAdd(v_indices, a_indices, index_count[sID], shared_sigma, stream_Call);
			// OuterProduct(v[i], a[i], temp_Mat[sID*4 + 2], stream_Call);
			// if(i%2 == 0)
			// 	ell_add(temp_Mat[sID*4 + 0], temp_Mat[sID*4 + 2], temp_Mat[sID*4 + 1], stream_Call);
			// else
			// 	ell_add(temp_Mat[sID*4 + 1], temp_Mat[sID*4 + 2], temp_Mat[sID*4 + 0], stream_Call);
		}
		//shared_sigma = (j%2 == 1) ? temp_Mat[sID*4 + 1] : temp_Mat[sID*4 + 0];
	}

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Call);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Call);
}

//f_call
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_call()
{
	fprintf(stdout, "f_call\n");
	for(int j=1; j<=m_maxCall; ++j)
	{		
		AND_OP<VALUE_TYPE> (r, Call[j], s[STREAM_CALL], stream_Call);
		f_call_device(s[STREAM_CALL], j, STREAM_CALL);
	}
}

//F_list
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_list_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j, const int sID)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	fprintf(stderr, "list: %d\n", entry_count);
	if(entry_count == 0)
		return;

	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_List);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_List);
	FILL<VALUE_TYPE> (v_list, 0, stream_List);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_List);
	for(int i=0; i < entry_count; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_List);
		ell_spmv(shared_sigma, Fun_vec[sID], vf[sID], stream_List);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_List);

		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_List);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_List);

		for(int k=0; k<j; ++k)
		{
			column_select(Arg[k], s_indices[sID], i, Arg_vec[sID], stream_List);
			ell_spmv(shared_sigma, Arg_vec[sID], v[k], stream_List);
			AccumVec<VALUE_TYPE> (v_list, v[k], stream_List);
		}
	}
	AccumVec<VALUE_TYPE> (v_list, LIST_vec, stream_List);

	gather_reduce(v_list, temp_row_indices[sID], index_count[sID], 0, stream_List);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_List);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], shared_sigma, stream_List);
	// OuterProduct(v_list, accum_var_vec[sID], temp_Mat[sID*4 + 0], stream_List);
	// ell_add(temp_Mat[sID*4 + 0], shared_sigma, temp_Mat[sID*4 + 1], stream_List);
	// shared_sigma = temp_Mat[sID*4 + 1];						//null stream

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_List);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_List);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_list()
{
	fprintf(stdout, "f_list\n");
	for(int j=0; j<=m_maxList; ++j)
	{
		AND_OP<VALUE_TYPE> (r, PrimList[j], s[STREAM_LIST], stream_List);
		f_list_device(s[STREAM_LIST], j, STREAM_LIST);
	}
}

// //F_set
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_set_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	fprintf(stderr, "set: %d\n", entry_count);
	if(entry_count == 0)
		return;

	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Set);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Set);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Set);
	//temp_Mat[sID*4 + 0] = shared_sigma;
	for(int i=0; i < entry_count; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Set);
		ell_spmv(shared_sigma, Fun_vec[sID], vf[sID], stream_Set);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Set);
		column_select(Arg[0], s_indices[sID], i, a_set, stream_Set);
		column_select(Arg[1], s_indices[sID], i, Arg_vec[sID], stream_Set);
		ell_spmv(shared_sigma, Arg_vec[sID], v_set, stream_Set);

		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Set);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Set);
		
		gather_reduce(v_set, temp_row_indices[sID], index_count[sID], 0, stream_Set);
		gather_reduce(a_set, temp_col_indices[sID], index_count[sID], 1, stream_Set);
		OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], shared_sigma, stream_Set);		
		// OuterProduct(v_set, a_set, temp_Mat[sID*4 + 1], stream_Set);
		// ell_add(temp_Mat[sID*4 + 0], temp_Mat[sID*4 + 1], temp_Mat[sID*4 + 2], stream_Set);
		// temp_Mat[sID*4 + 0] = temp_Mat[sID*4 + 2];
	}

	//sigma + (a_var (X) void) + (a_set (X) v_set)
	gather_reduce(VOID_vec, temp_row_indices[sID], index_count[sID], 0, stream_Set);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Set);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], shared_sigma, stream_Set);
	// OuterProduct(VOID_vec, accum_var_vec[sID], temp_Mat[sID*4 + 1], stream_Set);
	// ell_add(temp_Mat[sID*4 + 0], temp_Mat[sID*4 + 1], temp_Mat[sID*4 + 2], stream_Set);
	// shared_sigma = temp_Mat[sID*4 + 2];

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Set);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Set);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_set()
{
	fprintf(stdout, "f_set\n");
	AND_OP<VALUE_TYPE> (r, SET, s[STREAM_SET], stream_Set);
		
	f_set_device(s[STREAM_SET], STREAM_SET);
}

//F_if
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_if_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	fprintf(stderr, "if: %d\n", entry_count);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_IF);
	for(int i=0; i < entry_count; ++i)
	{
		column_select(Arg[0], s_indices[sID], i, Arg_vec[sID], stream_IF);
		ell_spmv(shared_sigma, Arg_vec[sID], v_cond, stream_IF);
	
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
	fprintf(stdout, "f_if\n");
	AND_OP<VALUE_TYPE> (r, IF, s[STREAM_IF], stream_IF);
		
	f_if_device(s[STREAM_IF], STREAM_IF);
}

//F_primNum
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primNum_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	fprintf(stderr, "num: %d\n", entry_count);
	if(entry_count == 0)
		return;

	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Num);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Num);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Num);
	for(int i=0; i < entry_count; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Num);
		ell_spmv(shared_sigma, Fun_vec[sID], vf[sID], stream_Num);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Num);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Num);
		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Num);
	}

	//sigma + (a_var (X) NUM)
	gather_reduce(NUM_vec, temp_row_indices[sID], index_count[sID], 0, stream_Num);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Num);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], shared_sigma, stream_Num);
	// OuterProduct(NUM_vec, accum_var_vec[sID], temp_Mat[sID*4 + 0], stream_Num);
	// ell_add(temp_Mat[sID*4 + 0], shared_sigma, temp_Mat[sID*4 + 1], stream_Num);
	// shared_sigma = temp_Mat[sID*4 + 1];

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Num);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Num);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primNum()
{
	fprintf(stdout, "f_primNum\n");
	AND_OP<VALUE_TYPE> (r, PrimNum, s[STREAM_NUM], stream_Num);
		
	f_primNum_device(s[STREAM_NUM], STREAM_NUM);
}

//F_primBool
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primBool_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	fprintf(stderr, "bool: %d\n", entry_count);
	if(entry_count == 0)
		return;

	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Bool);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Bool);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Bool);
	for(int i=0; i < entry_count; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Bool);
		ell_spmv(shared_sigma, Fun_vec[sID], vf[sID], stream_Bool);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Bool);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Bool);
		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Bool);
	}

	//sigma + (a_var (X) #T#F)
	gather_reduce(BOOL_vec, temp_row_indices[sID], index_count[sID], 0, stream_Bool);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Bool);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], shared_sigma, stream_Bool);
	// OuterProduct(BOOL_vec, accum_var_vec[sID], temp_Mat[sID*4 + 0], stream_Bool);
	// ell_add(temp_Mat[sID*4 + 0], shared_sigma, temp_Mat[sID*4 + 1], stream_Bool);
	// shared_sigma = temp_Mat[sID*4 + 1];

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Bool);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Bool);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primBool()
{
	fprintf(stdout, "f_primBool\n");
	AND_OP<VALUE_TYPE> (r, PrimBool, s[STREAM_BOOL], stream_Bool);

	f_primBool_device(s[STREAM_BOOL], STREAM_BOOL);
}

//F_primVoid
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primVoid_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int sID)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	fprintf(stderr, "void: %d\n", entry_count);
	if(entry_count == 0)
		return;

	FILL<VALUE_TYPE> (accum_vf_vec[sID], 0, stream_Void);
	FILL<VALUE_TYPE> (accum_var_vec[sID], 0, stream_Void);

	get_indices<VALUE_TYPE> (s, s_indices[sID], stream_Void);
	for(int i=0; i < entry_count; ++i)
	{
		column_select(Fun, s_indices[sID], i, Fun_vec[sID], stream_Void);
		ell_spmv(shared_sigma, Fun_vec[sID], vf[sID], stream_Void);
		csr_spmv(Var[0], vf[sID], a_var[sID], stream_Void);
		AccumVec<VALUE_TYPE> (accum_var_vec[sID], a_var[sID], stream_Void);
		AccumVec<VALUE_TYPE> (accum_vf_vec[sID], vf[sID], stream_Void);
	}

	//sigma + (a_var (X) VOID)
	gather_reduce(VOID_vec, temp_row_indices[sID], index_count[sID], 0, stream_Void);
	gather_reduce(accum_var_vec[sID], temp_col_indices[sID], index_count[sID], 1, stream_Void);
	OuterProductAdd(temp_row_indices[sID], temp_col_indices[sID], index_count[sID], shared_sigma, stream_Void);
	// OuterProduct(VOID_vec, accum_var_vec[sID], temp_Mat[sID*4 + 0], stream_Void);
	// ell_add(temp_Mat[sID*4 + 0], shared_sigma, temp_Mat[sID*4 + 1], stream_Void);
	// shared_sigma = temp_Mat[sID*4 + 1];

	//r_prime
	csr_spmv(Body, accum_vf_vec[sID], Body_vec[sID], stream_Void);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec[sID], stream_Void);
}

//entry point
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primVoid()
{
	fprintf(stdout, "f_PrimVoid\n");
	AND_OP<VALUE_TYPE> (r, PrimVoid, s[STREAM_VOID], stream_Void);
		
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