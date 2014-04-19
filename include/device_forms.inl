#if(BUILD_TYPE == GPU)

#include "primitives.h"

//*********************************************************************//
//Device forms

//F_call
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_call_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j)
{
	thrust::fill(accum_vf_vec.begin(), accum_vf_vec.end(), 0);

	int entry_count = thrust::count(s.begin(), s.end(), 1);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices, stream_Call);
	for(int index=0; index < entry_count; ++index)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (Fun, s_indices, index, Fun_vec, stream_Call);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Fun_vec, vf, stream_Call);

		AccumVec<VALUE_TYPE> (accum_vf_vec, vf, stream_Call);

		for(int i=0; i<j; ++i)
		{
			column_select<INDEX_TYPE, VALUE_TYPE> (Arg[i], s_indices, index, Arg_vec, stream_Call);
			ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Arg_vec, v[i], stream_Call);
			ell_spmv<INDEX_TYPE, VALUE_TYPE> (Var[i], vf, a[i], stream_Call);
		}
		
		temp_Mat[0] = sigma_prime;
		for(int i=0; i<j; ++i)
		{
			OuterProduct<INDEX_TYPE, VALUE_TYPE> (v[i], a[i], temp_Mat[2], stream_Call);
			if(i%2 == 0)
				ell_add<INDEX_TYPE> (temp_Mat[0], temp_Mat[2], temp_Mat[1], stream_Call);
			else
				ell_add<INDEX_TYPE> (temp_Mat[1], temp_Mat[2], temp_Mat[0], stream_Call);
		}
		sigma_prime = (j%2 == 1) ? temp_Mat[1] : temp_Mat[0];
	}

	//r_prime
	ell_spmv<INDEX_TYPE, VALUE_TYPE> (Body, accum_vf_vec, Body_vec, stream_Call);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec);		//null stream
}

//f_call
template <>
void CFA<int, char, cusp::device_memory>::f_call()
{
	fprintf(stdout, "f_call\n");
	for(int j=1; j<=m_maxCall; ++j)
	{		
		AND_OP<char> (r, Call[j], s, stream_Call);
		f_call_device(s, j);
	}
}

//F_list
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_list_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j)
{
	thrust::fill(accum_vf_vec.begin(), accum_vf_vec.end(), 0);
	thrust::fill(accum_var_vec.begin(), accum_var_vec.end(), 0);
	thrust::fill(v_list.begin(), v_list.end(), 0);

	//vf = s[i]
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices, stream_List);
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (Fun, s_indices, i, Fun_vec, stream_List);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Fun_vec, vf, stream_List);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (Var[0], vf, a_var, stream_List);

		AccumVec<VALUE_TYPE> (accum_vf_vec, vf, stream_List);
		AccumVec<VALUE_TYPE> (accum_var_vec, a_var, stream_List);

		for(int k=0; k<j; ++k)
		{
			column_select<INDEX_TYPE, VALUE_TYPE> (Arg[k], s_indices, i, Arg_vec, stream_List);
			ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Arg_vec, v[k], stream_List);
			AccumVec<VALUE_TYPE> (v_list, v[k], stream_List);
		}
	}
	AccumVec<VALUE_TYPE> (v_list, LIST_vec, stream_List);
	OuterProduct<INDEX_TYPE, VALUE_TYPE> (v_list, accum_var_vec, temp_Mat[0], stream_List);
	ell_add<INDEX_TYPE> (temp_Mat[0], sigma_prime, temp_Mat[1], stream_List);
	sigma_prime = temp_Mat[1];						//null stream

	//r_prime
	ell_spmv<INDEX_TYPE, VALUE_TYPE> (Body, accum_vf_vec, Body_vec, stream_List);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec);		//null stream
}

//entry point
template <>
void CFA<int, char, cusp::device_memory>::f_list()
{
	fprintf(stdout, "f_list\n");
	for(int j=0; j<=m_maxList; ++j)
	{
		AND_OP<char> (r, PrimList[j], s);
		f_list_device(s, j);
	}
}

// //F_set
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_set_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s)
{
	thrust::fill(accum_vf_vec.begin(), accum_vf_vec.end(), 0);
	thrust::fill(accum_var_vec.begin(), accum_var_vec.end(), 0);

	int entry_count = thrust::count(s.begin(), s.end(), 1);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices);
	temp_Mat[0] = sigma_prime;
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (Fun, s_indices, i, Fun_vec);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Fun_vec, vf);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (Var[0], vf, a_var);
		column_select<INDEX_TYPE, VALUE_TYPE> (Arg[0], s_indices, i, a_set);
		column_select<INDEX_TYPE, VALUE_TYPE> (Arg[1], s_indices, i, Arg_vec);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Arg_vec, v_set);

		AccumVec<VALUE_TYPE> (accum_vf_vec, vf);
		AccumVec<VALUE_TYPE> (accum_var_vec, a_var);
		
		OuterProduct<INDEX_TYPE, VALUE_TYPE> (v_set, a_set, temp_Mat[1]);
		ell_add<INDEX_TYPE> (temp_Mat[0], temp_Mat[1], temp_Mat[2]);
		temp_Mat[0] = temp_Mat[2];
	}

	//sigma + (a_var (X) void) + (a_set (X) v_set)
	OuterProduct<INDEX_TYPE, VALUE_TYPE> (VOID_vec, accum_var_vec, temp_Mat[1]);
	ell_add<INDEX_TYPE> (temp_Mat[0], temp_Mat[1], temp_Mat[2]);
	sigma_prime = temp_Mat[2];

	//r_prime
	ell_spmv<INDEX_TYPE, VALUE_TYPE> (Body, accum_vf_vec, Body_vec);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec);
}

//entry point
template <>
void CFA<int, char, cusp::device_memory>::f_set()
{
	fprintf(stdout, "f_set\n");
	AND_OP<char> (r, SET, s);
		
	f_set_device(s);
}

//F_if
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_if_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s)
{
	int entry_count = thrust::count(s.begin(), s.end(), 1);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices);
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (Arg[0], s_indices, i, Arg_vec);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Arg_vec, v_cond);
	
		InnerProductStore<VALUE_TYPE> (NOT_FALSE_vec, v_cond, AND_vec1, i);
		InnerProductStore<VALUE_TYPE> (FALSE_vec, v_cond, AND_vec2, i);
	}

	//TRUE
	AND_OP<VALUE_TYPE> (s, AND_vec1, temp_indices);
	entry_count = thrust::count(temp_indices.begin(), temp_indices.end(), 1);
	get_indices<VALUE_TYPE> (temp_indices, s_indices);
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (CondTrue, s_indices, i, Cond_vec);
		AccumVec<VALUE_TYPE> (r_prime, Cond_vec);
	}
	/////////////////////////////////////////////////////////////////////////////////

	//FALSE
	AND_OP<VALUE_TYPE> (s, AND_vec2, temp_indices);
	entry_count = thrust::count(temp_indices.begin(), temp_indices.end(), 1);
	get_indices<VALUE_TYPE> (temp_indices, s_indices);
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (CondFalse, s_indices, i, Cond_vec);
		AccumVec<VALUE_TYPE> (r_prime, Cond_vec);
	}
	/////////////////////////////////////////////////////////////////////////////////
}

//entry point
template <>
void CFA<int, char, cusp::device_memory>::f_if()
{
	fprintf(stdout, "f_if\n");
	AND_OP<char> (r, IF, s);
		
	f_if_device(s);
}

//F_primNum
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primNum_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s)
{
	thrust::fill(accum_vf_vec.begin(), accum_vf_vec.end(), 0);
	thrust::fill(accum_var_vec.begin(), accum_var_vec.end(), 0);

	int entry_count = thrust::count(s.begin(), s.end(), 1);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices);
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (Fun, s_indices, i, Fun_vec);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Fun_vec, vf);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (Var[0], vf, a_var);
		AccumVec<VALUE_TYPE> (accum_var_vec, a_var);
		AccumVec<VALUE_TYPE> (accum_vf_vec, vf);
	}

	//sigma + (a_var (X) NUM)
	OuterProduct<INDEX_TYPE, VALUE_TYPE> (NUM_vec, accum_var_vec, temp_Mat[0]);
	ell_add<INDEX_TYPE> (temp_Mat[0], sigma_prime, temp_Mat[1]);
	sigma_prime = temp_Mat[1];

	//r_prime
	ell_spmv<INDEX_TYPE, VALUE_TYPE> (Body, accum_vf_vec, Body_vec);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec);
}

//entry point
template <>
void CFA<int, char, cusp::device_memory>::f_primNum()
{
	fprintf(stdout, "f_primBool\n");
	AND_OP<char> (r, PrimNum, s);
		
	f_primNum_device(s);
}

//F_primBool
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primBool_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s)
{
	thrust::fill(accum_vf_vec.begin(), accum_vf_vec.end(), 0);
	thrust::fill(accum_var_vec.begin(), accum_var_vec.end(), 0);

	int entry_count = thrust::count(s.begin(), s.end(), 1);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices);
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (Fun, s_indices, i, Fun_vec);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Fun_vec, vf);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (Var[0], vf, a_var);
		AccumVec<VALUE_TYPE> (accum_var_vec, a_var);
		AccumVec<VALUE_TYPE> (accum_vf_vec, vf);
	}

	//sigma + (a_var (X) #T#F)
	OuterProduct<INDEX_TYPE, VALUE_TYPE> (BOOL_vec, accum_var_vec, temp_Mat[0]);
	ell_add<INDEX_TYPE> (temp_Mat[0], sigma_prime, temp_Mat[1]);
	sigma_prime = temp_Mat[1];

	//r_prime
	ell_spmv<INDEX_TYPE, VALUE_TYPE> (Body, accum_vf_vec, Body_vec);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec);
}

//entry point
template <>
void CFA<int, char, cusp::device_memory>::f_primBool()
{
	fprintf(stdout, "f_primBool\n");
	AND_OP<char> (r, PrimBool, s);
		
	f_primBool_device(s);
}

//F_primVoid
template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::f_primVoid_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s)
{
	thrust::fill(accum_vf_vec.begin(), accum_vf_vec.end(), 0);
	thrust::fill(accum_var_vec.begin(), accum_var_vec.end(), 0);

	int entry_count = thrust::count(s.begin(), s.end(), 1);
	if(entry_count == 0)
		return;

	get_indices<VALUE_TYPE> (s, s_indices);
	for(int i=0; i < entry_count; ++i)
	{
		column_select<INDEX_TYPE, VALUE_TYPE> (Fun, s_indices, i, Fun_vec);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (sigma_prime, Fun_vec, vf);
		ell_spmv<INDEX_TYPE, VALUE_TYPE> (Var[0], vf, a_var);
		AccumVec<VALUE_TYPE> (accum_var_vec, a_var);
		AccumVec<VALUE_TYPE> (accum_vf_vec, vf);
	}

	//sigma + (a_var (X) VOID)
	OuterProduct<INDEX_TYPE, VALUE_TYPE> (VOID_vec, accum_var_vec, temp_Mat[0]);
	ell_add<INDEX_TYPE> (temp_Mat[0], sigma_prime, temp_Mat[1]);
	sigma_prime = temp_Mat[1];

	//r_prime
	ell_spmv<INDEX_TYPE, VALUE_TYPE> (Body, accum_vf_vec, Body_vec);
	AccumVec<VALUE_TYPE> (r_prime, Body_vec);
}

// //entry point
template <>
void CFA<int, char, cusp::device_memory>::f_primVoid()
{
	fprintf(stdout, "f_PrimVoid\n");
	AND_OP<char> (r, PrimVoid, s);
		
	f_primVoid_device(s);
}

#endif