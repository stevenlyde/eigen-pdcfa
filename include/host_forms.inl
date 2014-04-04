//F_call
// template <typename INDEX_TYPE, typename MEM_TYPE>
// void CFA<INDEX_TYPE, MEM_TYPE>::f_call(const CuspVectorChar_d &s, const int j)
// {
// 	//vf = s[i]
// 	cusp::multiply(s, Fun, temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], sigma_prime, vf);

// 	for(int i=0; i<m_maxArgs; ++i)
// 	{
// 		cusp::multiply(s, Arg[i], temp_Mat[0]);
// 		cusp::multiply(temp_Mat[0], sigma_prime, v[i]);
// 	}

// 	for(int i=0; i<m_maxArgs; ++i)
// 	{
// 		cusp::multiply(vf, Var[i], temp_Mat[0]);
// 		cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 		cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 		Vectorize(temp_Mat[2], a[i]);
// 	}

// 	temp_Mat[0] = sigma_prime;
// 	for(int i=0; i<j; ++i)
// 	{
// 		cusp::transpose(a[i], temp_Mat[3]);
// 		cusp::multiply(temp_Mat[3], v[i], temp_Mat[2]);
// 		temp_Mat[2].resize(m_ValuesSize, temp_Mat[2].num_cols, temp_Mat[2].num_entries, 32);
// 		if(i%2 == 0)
// 			cusp::add(temp_Mat[0], temp_Mat[2], temp_Mat[1]);
// 		else
// 			cusp::add(temp_Mat[1], temp_Mat[2], temp_Mat[0]);
// 	}
// 	sigma_prime = (j%2 == 1) ? temp_Mat[1] : temp_Mat[0];

// 	cusp::multiply(vf, Body, temp_Mat[0]);
// 	cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 	cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	//Vectorize(temp_Mat[2], temp_Mat[3]);
// 	cusp::add(temp_Mat[3], r_prime, temp_Mat[0]);
// 	r_prime = temp_Mat[0];
// }


//F_set
template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::f_set(const cusp::array1d<INDEX_TYPE, MEM_TYPE> &s)
{
	// cusp::array1d<INDEX_TYPE, MEM_TYPE> temp_vec;
	// cusp::multiply(Fun, s, temp_vec);
	// cusp::multiply(sigma_prime, temp_vec, vf);

	// cusp::multiply(vf, Var[0], temp_Mat[0]);
	// cusp::transpose(temp_Mat[0], temp_Mat[1]);
	// cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
	// Vectorize(temp_Mat[2], a_var);
	
	// cusp::multiply(s, Arg[0], a_set);
	// cusp::multiply(s, Arg[1], temp_Mat[0]);
	// cusp::multiply(temp_Mat[0], sigma_prime, v_set);
	// DEBUG_PRINT("a_set:\n", a_set);
	// DEBUG_PRINT("v_set:\n", v_set);

	// //sigma + (a_var (X) void) + (a_set (X) v_set)
	// cusp::transpose(a_var, temp_Mat[0]);
	// cusp::multiply(temp_Mat[0], VOID_vec, temp_Mat[1]);
	// temp_Mat[1].resize(m_ValuesSize, temp_Mat[1].num_cols, temp_Mat[1].num_entries, 32);
	// cusp::transpose(a_set, temp_Mat[0]);
	// cusp::multiply(temp_Mat[0], v_set, temp_Mat[2]);
	// temp_Mat[2].resize(m_ValuesSize, temp_Mat[2].num_cols, temp_Mat[2].num_entries, 32);
	// cusp::add(temp_Mat[1], sigma_prime, temp_Mat[3]);
	// cusp::add(temp_Mat[2], temp_Mat[3], sigma_prime);

	// //r_prime
	// cusp::multiply(vf, Body, temp_Mat[0]);
	// cusp::transpose(temp_Mat[0], temp_Mat[1]);
	// cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
	// Vectorize(temp_Mat[2], temp_Mat[3]);
	// cusp::add(temp_Mat[3], r_prime, temp_Mat[0]);
	// r_prime = temp_Mat[0];
}

//F_if
// template <typename INDEX_TYPE, typename MEM_TYPE>
// void CFA<INDEX_TYPE, MEM_TYPE>::f_if(const CuspVectorChar_d &s)
// {
// 	cusp::multiply(s, Arg[0], temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], sigma_prime, v_cond);
// 	DEBUG_PRINT("v_cond:\n", v_cond);

// 	cusp::multiply(v_cond, NOT_FALSE_vec, tb);
// 	cusp::multiply(v_cond, FALSE_vec, fb);
// 	DEBUG_PRINT("tb:\n", tb);
// 	DEBUG_PRINT("fb:\n", fb);

// 	if(tb[0] == 1 && fb[0] == 1)
// 	{
// 		cusp::multiply(s, CondTrue, temp_Mat[0]);
// 		cusp::multiply(s, CondFalse, temp_Mat[1]);
// 		cusp::add(temp_Mat[0], r_prime, temp_Mat[2]);
// 		cusp::add(temp_Mat[1], temp_Mat[2], r_prime);
// 	}
// 	else if(tb[0] == 1)
// 	{	
// 		cusp::multiply(s, CondTrue, temp_Mat[0]);
// 		cusp::add(r_prime, temp_Mat[0], temp_Mat[1]);
// 		r_prime = temp_Mat[1];
// 	}
// 	else if(fb[0] == 1)
// 	{
// 		cusp::multiply(s, CondFalse, temp_Mat[0]);
// 		cusp::add(r_prime, temp_Mat[0], temp_Mat[1]);
// 		r_prime = temp_Mat[1];
// 	}
// }

// //F_primNum
// template <typename INDEX_TYPE, typename MEM_TYPE>
// void CFA<INDEX_TYPE, MEM_TYPE>::f_primNum(const INDEX_TYPE &s)
// {
// 	cusp::multiply(s, Fun, temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], sigma_prime, vf);

// 	cusp::multiply(vf, Var[0], temp_Mat[0]);
// 	cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 	cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], a_var);

// 	//sigma + (a_var (X) NUM)
// 	cusp::transpose(a_var, temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], NUM_vec, temp_Mat[1]);
// 	temp_Mat[1].resize(m_ValuesSize, temp_Mat[1].num_cols, temp_Mat[1].num_entries, 32);
// 	cusp::add(temp_Mat[1], sigma_prime, temp_Mat[3]);
// 	sigma_prime = temp_Mat[3];

// 	//r_prime
// 	cusp::multiply(vf, Body, temp_Mat[0]);
// 	cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 	cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], temp_Mat[3]);
// 	cusp::add(temp_Mat[3], r_prime, temp_Mat[0]);
// 	r_prime = temp_Mat[0];
// }

// //F_primBool
// template <typename INDEX_TYPE, typename MEM_TYPE>
// void CFA<INDEX_TYPE, MEM_TYPE>::f_primBool(const INDEX_TYPE &s)
// {
// 	cusp::multiply(s, Fun, temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], sigma_prime, vf);

// 	cusp::multiply(vf, Var[0], temp_Mat[0]);
// 	cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 	cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], a_var);

// 	//sigma + (a_var (X) #T#F)
// 	cusp::transpose(a_var, temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], BOOL_vec, temp_Mat[1]);
// 	temp_Mat[1].resize(m_ValuesSize, temp_Mat[1].num_cols, temp_Mat[1].num_entries, 32);
// 	cusp::add(temp_Mat[1], sigma_prime, temp_Mat[3]);
// 	sigma_prime = temp_Mat[3];

// 	//r_prime
// 	cusp::multiply(vf, Body, temp_Mat[0]);
// 	cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 	cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], temp_Mat[3]);
// 	cusp::add(temp_Mat[3], r_prime, temp_Mat[0]);
// 	r_prime = temp_Mat[0];
// }

// //F_primVoid
// template <typename INDEX_TYPE, typename MEM_TYPE>
// void CFA<INDEX_TYPE, MEM_TYPE>::f_primVoid(const INDEX_TYPE &s)
// {
// 	cusp::multiply(s, Fun, temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], sigma_prime, vf);
// 	DEBUG_PRINT("vf:\n", vf);

// 	cusp::multiply(vf, Var[0], temp_Mat[0]);
// 	cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 	cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], a_var);
// 	DEBUG_PRINT("a_var:\n", a_var);

// 	//sigma + (a_var (X) VOID)
// 	cusp::transpose(a_var, temp_Mat[0]);
// 	cusp::multiply(temp_Mat[0], VOID_vec, temp_Mat[1]);
// 	temp_Mat[1].resize(m_ValuesSize, temp_Mat[1].num_cols, temp_Mat[1].num_entries, 32);
// 	cusp::add(temp_Mat[1], sigma_prime, temp_Mat[3]);
// 	sigma_prime = temp_Mat[3];
// 	DEBUG_PRINT("sigma_prime:\n", sigma_prime);

// 	//r_prime
// 	cusp::multiply(vf, Body, temp_Mat[0]);
// 	cusp::transpose(temp_Mat[0], temp_Mat[1]);
// 	cusp::multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], temp_Mat[3]);
// 	DEBUG_PRINT("temp_Mat[3]:\n", temp_Mat[3]);
// 	cusp::add(temp_Mat[3], r_prime, temp_Mat[0]);
// 	r_prime = temp_Mat[0];
// }
