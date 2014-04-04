//F_call
// <typename INDEX_TYPE, typename MEM_TYPE>
// void CFA<INDEX_TYPE, cusp::device_memory>::f_call(const INDEX_TYPE s, const int j)
// {
// 	// row_multiply(s, Fun, sigma_prime, vf);

// 	// for(int i=0; i<m_maxArgs; ++i)
// 	// {
// 	// 	row_multiply(s, Arg[i], sigma_prime, v[i]);
// 	// 	multiply(Var[i], vf, a[i]);
// 	// }

// 	// //update sigma_prime
// 	// for(int i=0; i<j; ++i)
// 	// {
// 	// 	outer_product_add(a[i], v[i], sigma_prime);
// 	// }

// 	// //update r_prime
// 	// multiply_add(Body, vf, r_prime);
// }

// //F_set
// template <typename INDEX_TYPE>
// void CFA<INDEX_TYPE, device>::f_set(const INDEX_TYPE s)
// {
// 	row_multiply(s, Fun, sigma_prime, vf);
// 	multiply(Var[0], vf, a_var);
// 	column_select(Arg[0], s, a_set);
// 	row_multiply(s, Arg[1], sigma_prime, v_set);

// 	//update sigma_prime
// 	outer_product_add(a_var, VOID_vec, sigma_prime);
// 	outer_product_add(a_set, v_set, sigma_prime);

// 	//update r_prime
// 	multiply_add(Body, vf, r_prime);
// }

// //F_if
// template <typename INDEX_TYPE, typename SYSTEM_TYPE>
// void CFA<INDEX_TYPE, device>::f_if(const INDEX_TYPE s)
// {
// 	row_multiply(s, Arg[0], sigma_prime, v_cond);

// 	inner_product(v_cond, NOT_FALSE_vec, tb);
// 	inner_product(v_cond, FALSE_vec, fb);

// 	column_select(CondTrue, s, temp_vec[0]);
// 	column_select(CondFalse, s, temp_vec[1]);
	
// 	add(tb, temp_vec[0], fb, temp_vec[1], r_prime);
// }

// //F_primNum
// template <typename INDEX_TYPE, typename SYSTEM_TYPE>
// void CFA<INDEX_TYPE, device>::f_primNum(const INDEX_TYPE s)
// {
// 	multiply(s, Fun, temp_Mat[0]);
// 	multiply(temp_Mat[0], sigma_prime, vf);

// 	multiply(vf, Var[0], temp_Mat[0]);
// 	multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], a_var);

// 	//sigma + (a_var (X) NUM)
// 	multiply(temp_Mat[0], NUM_vec, temp_Mat[1]);
// 	temp_Mat[1].resize(m_ValuesSize, temp_Mat[1].num_cols, temp_Mat[1].num_entries);
// 	add(temp_Mat[1], sigma_prime, temp_Mat[3]);
// 	sigma_prime = temp_Mat[3];

// 	//update r_prime
// 	multiply_add(Body, vf, r_prime);
// }

// //F_primBool
// template <typename INDEX_TYPE, typename SYSTEM_TYPE>
// void CFA<INDEX_TYPE, device>::f_primBool(const INDEX_TYPE s)
// {
// 	multiply(s, Fun, temp_Mat[0]);
// 	multiply(temp_Mat[0], sigma_prime, vf);

// 	multiply(vf, Var[0], temp_Mat[0]);
// 	multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], a_var);

// 	//sigma + (a_var (X) #T#F)
// 	multiply(temp_Mat[0], BOOL_vec, temp_Mat[1]);
// 	temp_Mat[1].resize(m_ValuesSize, temp_Mat[1].num_cols, temp_Mat[1].num_entries);
// 	add(temp_Mat[1], sigma_prime, temp_Mat[3]);
// 	sigma_prime = temp_Mat[3];

// 	//update r_prime
// 	multiply_add(Body, vf, r_prime);
// }

// //F_primVoid
// template <typename INDEX_TYPE, typename SYSTEM_TYPE>
// void CFA<INDEX_TYPE, device>::f_primVoid(const INDEX_TYPE s)
// {
// 	multiply(s, Fun, temp_Mat[0]);
// 	multiply(temp_Mat[0], sigma_prime, vf);

// 	multiply(vf, Var[0], temp_Mat[0]);
// 	multiply(temp_Mat[1], temp_Mat[7], temp_Mat[2]);
// 	Vectorize(temp_Mat[2], a_var);

// 	//sigma + (a_var (X) VOID)
// 	multiply(temp_Mat[0], VOID_vec, temp_Mat[1]);
// 	temp_Mat[1].resize(m_ValuesSize, temp_Mat[1].num_cols, temp_Mat[1].num_entries);
// 	add(temp_Mat[1], sigma_prime, temp_Mat[3]);
// 	sigma_prime = temp_Mat[3];
// 	DEBUG_PRINT("sigma_prime:\n", sigma_prime);

// 	//update r_prime
// 	multiply_add(Body, vf, r_prime);
// }
