#if(BUILD_TYPE == GPU)

typedef struct __mat_info
{
	int num_rows;
	int num_cols;
	int num_entries;
	int num_cols_per_row;
	int pitch;
} mat_info;

template <typename INDEX_TYPE>
inline void get_ellmatrix_info(	cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory> &mat, mat_info &info)
{
	info.num_rows = mat.num_rows;
	info.num_cols = mat.num_cols;
	info.num_entries = mat.num_entries;
	info.num_cols_per_row = mat.column_indices.num_cols;
	info.pitch = mat.column_indices.pitch;
}

template <typename INDEX_TYPE>
inline void print_matrix_info(cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory> &mat)
{
	mat_info info;
	get_ellmatrix_info(mat, info);
	fprintf(stderr, "rows: %d  cols: %d  num_entries: %d  num_cols_per_row: %d  pitch: %d\n", 
		info.num_rows, info.num_cols, info.num_entries, info.num_cols_per_row, info.pitch);
}

template <typename INDEX_TYPE>
inline __device__ void reduce_sum(INDEX_TYPE *a, const int size, const int tID);

template <typename INDEX_TYPE>
inline __device__ void reduce_sum(INDEX_TYPE *a, const int size, const int tID)
{
	if(tID < 256)
		a[tID] += a[tID + 256];
	__syncthreads();
	if(tID < 128)
		a[tID] += a[tID + 128];
	__syncthreads();
	if(tID < 64)
		a[tID] += a[tID + 64];
	__syncthreads();
	if(tID < 32)
		a[tID] += a[tID + 32];
	__syncthreads();
	if(tID < 16)
		a[tID] += a[tID + 16];
	__syncthreads();
	if(tID < 8)
		a[tID] += a[tID + 8];
	__syncthreads();
	if(tID < 4)
		a[tID] += a[tID + 4];
	__syncthreads();
	if(tID < 2)
		a[tID] += a[tID + 2];
	__syncthreads();
	if(tID < 1)
		a[tID] += a[tID + 1];
	__syncthreads();
}

template <typename INDEX_TYPE>
__launch_bounds__(BLOCK_SIZE,1)
__global__ void AND_OP(	const INDEX_TYPE *A,
						const INDEX_TYPE *B,
						INDEX_TYPE *C,
						const INDEX_TYPE size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	for(int i=tID; i<size; i+=grid_size)
		C[i] = A[i] & B[i];
}

template <typename INDEX_TYPE>
__launch_bounds__(BLOCK_SIZE,1)
__global__ void AccumVec(	INDEX_TYPE *a,
							const INDEX_TYPE *b,
							const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	//a += b
	for(int i=tID; i<size; i+=grid_size)
		a[i] += b[i];
}

template <typename INDEX_TYPE>
__launch_bounds__(BLOCK_SIZE,1)
__global__ void AccumMat(	const 	INDEX_TYPE num_rows,
							const 	INDEX_TYPE num_cols,
							const 	INDEX_TYPE num_cols_per_row,
							const 	INDEX_TYPE pitch,
							const 	INDEX_TYPE *column_indices,
									INDEX_TYPE *a)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

	for(int row=tID; row < num_rows; row+=grid_size)
	{
		INDEX_TYPE offset = row;
		for(int n=0; n < num_cols_per_row; ++n)
		{
			int col = column_indices[offset];
			if(col != invalid_index)
			{
				a[row] = 1;
			}
			offset += pitch;
		}
	}
}

template <typename INDEX_TYPE>
__launch_bounds__(BLOCK_SIZE,1)
__global__ void 
column_select(	const 	INDEX_TYPE num_rows,
				const 	INDEX_TYPE num_cols,
				const 	INDEX_TYPE num_cols_per_row,
				const 	INDEX_TYPE pitch,
				const	INDEX_TYPE *A_column_indices,
				const 	INDEX_TYPE *x,
						INDEX_TYPE *B_column_indices,
						INDEX_TYPE *B_values)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

	for(int row=tID; row < num_rows; row+=grid_size)
	{
		INDEX_TYPE offset = row;
		for(int n=0; n < num_cols_per_row; ++n)
		{
			int col = A_column_indices[offset];
			if(col != invalid_index)
			{
				if(x[col])
				{
					B_column_indices[offset] = col;
					B_values[offset] = 1;
					offset += pitch;
				}
			}
		}

		while(offset < num_cols_per_row*pitch)
		{
			B_column_indices[offset] = invalid_index;
			offset += pitch;
		}
	}
}

template <typename INDEX_TYPE>
__launch_bounds__(BLOCK_SIZE,1)
__global__ void OuterProduct(	const INDEX_TYPE *a,
								const INDEX_TYPE *b,
								const INDEX_TYPE size_a,
								const INDEX_TYPE size_b,
								const INDEX_TYPE num_rows,
								const INDEX_TYPE num_cols,
								const INDEX_TYPE num_cols_per_row,
								const INDEX_TYPE pitch,
								INDEX_TYPE *column_indices,
								INDEX_TYPE *values)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;
	
	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

	__shared__ int entries_b[BLOCK_SIZE];
	__shared__ int num_entries_a, num_entries_b;

	entries_b[threadIdx.x] = -1;
	__syncthreads();

	if(threadIdx.x == 0)		//first thread of every block
	{
		num_entries_a = 0;
		num_entries_b = 0;

		for(int i=0; i < size_a; ++i)
		{
			if(a[i] != 0)
				num_entries_a++;
		}

		for(int i=0; i < size_b; ++i)
		{
			if(b[i] != 0)
			{
				entries_b[num_entries_b] = i;
				num_entries_b++;
			}
		}
	}
	__syncthreads();

	for(int row=tID; row<size_a; row+=grid_size)
	{
		int offset = row;
		if(a[row])
		{
			for(int n=0; n < num_entries_b; ++n, offset+=pitch)
			{
				column_indices[offset] = entries_b[n];
				values[offset] = 1;
			}
		}

		while(offset < num_cols_per_row*pitch)
		{
			column_indices[offset] = invalid_index;
			offset += pitch;
		}
	}
}

template <typename INDEX_TYPE>
__launch_bounds__(BLOCK_SIZE,1)
__global__ void ell_add(	const INDEX_TYPE num_rows,
							const INDEX_TYPE num_cols,
							const INDEX_TYPE A_num_cols_per_row,
							const INDEX_TYPE B_num_cols_per_row,
							const INDEX_TYPE C_num_cols_per_row,
							const INDEX_TYPE A_pitch,
							const INDEX_TYPE B_pitch,
							const INDEX_TYPE C_pitch,
							const INDEX_TYPE *A_column_indices,
							const INDEX_TYPE *B_column_indices,
							INDEX_TYPE *C_column_indices,
							INDEX_TYPE *C_values)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	//__shared__ int entry_count[BLOCK_SIZE];
	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

	//entry_count[tID] = 0;

	for(int row=tID; row < num_rows; row+=grid_size)
	{
		int a_index = row;
		int b_index = row;
		int c_index = row;
		int A_col = A_column_indices[a_index];
		int B_col = B_column_indices[b_index];
		for(int n=0; n < C_num_cols_per_row; ++n, c_index+=C_pitch)
		{
			if(A_col == invalid_index && B_col == invalid_index)
			{
				C_column_indices[c_index] = invalid_index;
			}
			else if(A_col != invalid_index && B_col == invalid_index)
			{
				C_column_indices[c_index] = A_col;
				C_values[c_index] = 1;
				
				a_index += A_pitch;

				if(a_index < A_num_cols_per_row * A_pitch)
					A_col = A_column_indices[a_index];
				else
					A_col = invalid_index;
				//entry_count[tID]++;
			}
			else if(A_col == invalid_index && B_col != invalid_index)
			{
				C_column_indices[c_index] = B_col;
				C_values[c_index] = 1;
				
				b_index += B_pitch;

				if(b_index < B_num_cols_per_row * B_pitch)
					B_col = B_column_indices[b_index];
				else
					B_col = invalid_index;
				//entry_count[tID]++;
			}
			else if(A_column_indices[a_index] < B_column_indices[b_index])
			{
				C_column_indices[c_index] = A_col;
				C_values[c_index] = 1;
				
				a_index += A_pitch;

				if(a_index < A_num_cols_per_row * A_pitch)
					A_col = A_column_indices[a_index];
				else
					A_col = invalid_index;
				//entry_count[tID]++;
			}
			else if(B_column_indices[b_index] < A_column_indices[a_index])
			{
				C_column_indices[c_index] = B_col;
				C_values[c_index] = 1;
				
				b_index += B_pitch;

				if(b_index < B_num_cols_per_row * B_pitch)
					B_col = B_column_indices[b_index];
				else
					B_col = invalid_index;
				//entry_count[tID]++;
			}
			else if(A_column_indices[a_index] == B_column_indices[b_index])
			{
				C_column_indices[c_index] = A_col;
				C_values[c_index] = 1;
				
				a_index += A_pitch;
				b_index += B_pitch;

				if(a_index < A_num_cols_per_row * A_pitch)
					A_col = A_column_indices[a_index];
				else
					A_col = invalid_index;
				if(b_index < B_num_cols_per_row * B_pitch)
					B_col = B_column_indices[b_index];
				else
					B_col = invalid_index;
				//entry_count[tID]++;
			}
		}

		// while(c_index < C_num_cols_per_row * C_pitch)
		// {
		// 	C_column_indices[c_index] = invalid_index;
		// 	c_index += C_pitch;
		// }
	}
	__syncthreads();

	// reduce_sum<INDEX_TYPE>(entry_count, BLOCK_SIZE, tID);
	// if(tID == 0)
	// 	*C_num_entries = entry_count[0];
}

//*********************************************************************//
//Device forms

//F_call
template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::f_call_device(const cusp::array1d<INDEX_TYPE, cusp::device_memory> &s, const int j)
{
	mat_info info1, info2, info3;
	get_ellmatrix_info(Fun, info1);
	column_select<INDEX_TYPE> <<<GRID, BLOCK>>> (	info1.num_rows,
													info1.num_cols,
													info1.num_cols_per_row,
													info1.pitch,
													TPC(&Fun.column_indices.values[0]), 
													TPC(&s[0]),
													TPC(&temp_Fun.column_indices.values[0]), 
													TPC(&temp_Fun.values.values[0]));
	cusp::multiply(sigma_prime, temp_Fun, vf_mat);

	get_ellmatrix_info(Arg[0], info1);
	for(int i=0; i<m_maxCall; ++i)
	{
		column_select<INDEX_TYPE> <<<GRID, BLOCK>>>(	info1.num_rows,
														info1.num_cols,
														info1.num_cols_per_row,
														info1.pitch,
														TPC(&Arg[i].column_indices.values[0]),
														TPC(&s[0]),
														TPC(&temp_Arg.column_indices.values[0]), 
														TPC(&temp_Arg.values.values[0]));
		cusp::multiply(sigma_prime, temp_Arg, v_mat[i]);
	}

	for(int i=0; i<m_maxCall; ++i)
	{
		cusp::multiply(Var[i], vf_mat, a_mat[i]);
	}

	temp_Mat[0] = sigma_prime;
	get_ellmatrix_info(temp_Mat[0], info1);
	get_ellmatrix_info(temp_Mat[1], info2);
	get_ellmatrix_info(temp_Mat[2], info3);
	for(int i=0; i<j; ++i)
	{
		OuterProduct<INDEX_TYPE> <<<GRID, BLOCK>>> (TPC(&v[i][0]), 
													TPC(&a[i][0]), 
													v[i].size(), 
													a[i].size(), 
													info1.num_rows,
													info1.num_cols,
													info1.num_cols_per_row,
													info1.pitch,
													TPC(&temp_Mat[2].column_indices.values[0]),
													TPC(&temp_Mat[2].values.values[0]));
		if(i%2 == 0)
			ell_add<INDEX_TYPE> <<<GRID, BLOCK>>> (	info1.num_rows,
													info1.num_cols,
													info1.num_cols_per_row,
													info3.num_cols_per_row,
													info2.num_cols_per_row,
													info1.pitch,
													info3.pitch,
													info2.pitch,
													TPC(&temp_Mat[0].column_indices.values[0]),
													TPC(&temp_Mat[2].column_indices.values[0]),
													TPC(&temp_Mat[1].column_indices.values[0]),
													TPC(&temp_Mat[1].values.values[0]));
		else
			ell_add<INDEX_TYPE> <<<GRID, BLOCK>>> (	info1.num_rows,
													info1.num_cols,
													info2.num_cols_per_row,
													info3.num_cols_per_row,
													info1.num_cols_per_row,
													info2.pitch,
													info3.pitch,
													info1.pitch,
													TPC(&temp_Mat[1].column_indices.values[0]),
													TPC(&temp_Mat[2].column_indices.values[0]),
													TPC(&temp_Mat[0].column_indices.values[0]),
													TPC(&temp_Mat[0].values.values[0]));
	}
	sigma_prime = (j%2 == 1) ? temp_Mat[1] : temp_Mat[0];

	//r_prime
	cusp::multiply(Body, vf_mat, temp_Body);
	get_ellmatrix_info(temp_Body, info1);
	AccumMat <<<GRID, BLOCK>>> (info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info1.pitch,
								TPC(&temp_Body.column_indices.values[0]),
								TPC(&r_prime[0]));
}

//f_call
template <>
void CFA<int, cusp::device_memory>::f_call()
{
	for(int j=1; j<=m_maxCall; ++j)
	{
		fprintf(stdout, "f_list_%d\n", j);
		thrust::fill(AND_vec.begin(), AND_vec.end(), 0);
		AND_OP <<<GRID, BLOCK>>> (	TPC(&r[0]), 
									TPC(&Call[j][0]), 
									TPC(&AND_vec[0]), 
									int(r.size()));

		f_call_device(AND_vec, j);
	}
}

// //F_list
//template <typename INDEX_TYPE, typename MEM_TYPE>
//void CFA<INDEX_TYPE, MEM_TYPE>::f_list_device(const cusp::array1d<INDEX_TYPE, cusp::device_memory> &s, const int j)
// {
// 	//vf = s[i]
// 	temp_vec.resize(Fun.num_rows);
// 	cusp::multiply(Fun, s, temp_vec);
// 	cusp::multiply(sigma_prime, temp_vec, vf);
// 	cusp::multiply(Var[0], vf, a_var);

// 	temp_vec.resize(Arg[0].num_rows);
// 	thrust::fill(v_list.begin(), v_list.end(), 0);
// 	for(int i=0; i<j; ++i)
// 	{
// 		cusp::multiply(Arg[i], s, temp_vec);
// 		cusp::multiply(sigma_prime, temp_vec, v[i]);
// 		CFA::AccumVec(v_list, v[i]);
// 	}
// 	CFA::AccumVec(v_list, LIST_vec);
// 	OuterProduct(v_list, a_var, temp_Mat[0]);
// 	cusp::add(temp_Mat[0], sigma_prime, temp_Mat[1]);
// 	sigma_prime = temp_Mat[1];

// 	//r_prime
// 	temp_vec.resize(Body.num_rows);
// 	cusp::multiply(Body, vf, temp_vec);
// 	CFA::AccumVec(r_prime, temp_vec);
// }

//entry point
template <>
void CFA<int, cusp::device_memory>::f_list()
{
	// std::vector<int> search_vec;
	// for(int j=0; j<=m_maxList; ++j)
	// {
	// 	AND_OP(r, PrimList[j], search_vec);
	// 	fprintf(stdout, "f_list_%d: %d\n", j, search_vec.size());

	// 	for(int i=0; i<search_vec.size(); ++i)
	// 	{
	// 		thrust::fill(s.begin(), s.end(), 0);
	// 		s[search_vec[i]] = 1;
	// 		f_list_host(s, j);
	// 	}
	// }
}

// //F_set
//template <typename INDEX_TYPE, typename MEM_TYPE>
//void CFA<INDEX_TYPE, MEM_TYPE>::f_set_device(const int s)
// {
// 	temp_vec.resize(Fun.num_rows);
// 	cusp::multiply(Fun, s, temp_vec);
// 	cusp::multiply(sigma_prime, temp_vec, vf);
// 	cusp::multiply(Var[0], vf, a_var);
// 	cusp::multiply(Arg[0], s, a_set);
// 	temp_vec.resize(Arg[1].num_rows);
// 	cusp::multiply(Arg[1], s, temp_vec);
// 	cusp::multiply(sigma_prime, temp_vec, v_set);

// 	//sigma + (a_var (X) void) + (a_set (X) v_set)
// 	cusp::add(CFA::OuterProduct(VOID_vec, a_var, temp_Mat[0]), CFA::OuterProduct(v_set, a_set, temp_Mat[1]), temp_Mat[2]);
// 	cusp::add(temp_Mat[2], sigma_prime, temp_Mat[3]);
// 	sigma_prime = temp_Mat[3];

// 	//r_prime
// 	temp_vec.resize(Body.num_rows);
// 	cusp::multiply(Body, vf, temp_vec);
// 	CFA::AccumVec(r_prime, temp_vec);
// }

//entry point
template <>
void CFA<int, cusp::device_memory>::f_set()
{
	// std::vector<int> search_vec;
	// AND_OP(r, SET, search_vec);
	// fprintf(stdout, "f_set: %d\n", search_vec.size());
	// for(int i=0; i<search_vec.size(); ++i)
	// {
	// 	thrust::fill(s.begin(), s.end(), 0);
	// 	s[search_vec[i]] = 1;
	// 	f_set_host(s);
	// }
}

//F_if
template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::f_if_device(const cusp::array1d<INDEX_TYPE, cusp::device_memory> &s)
{
	mat_info info1, info2, info3;
	get_ellmatrix_info(Arg[0], info1);
	column_select<INDEX_TYPE> <<<GRID, BLOCK>>> (	info1.num_rows,
													info1.num_cols,
													info1.num_cols_per_row,
													info1.pitch,
													TPC(&Arg[0].column_indices.values[0]), 
													TPC(&s[0]),
													TPC(&temp_Arg.column_indices.values[0]), 
													TPC(&temp_Arg.values.values[0]));
	cusp::multiply(sigma_prime, temp_Arg, v_cond_mat);

	temp_vec.resize(Arg[0].num_rows);
	cusp::multiply(Arg[0], s, temp_vec);
	cusp::multiply(sigma_prime, temp_vec, v_cond);

	int tb = thrust::inner_product(v_cond.begin(), v_cond.end(), NOT_FALSE_vec.begin(), 0);
	int fb = thrust::inner_product(v_cond.begin(), v_cond.end(), FALSE_vec.begin(), 0);

	//fprintf(stderr, "tb: %d  fb: %d\n", tb, fb);
	temp_vec.resize(CondTrue.num_rows);
	if(tb && fb)
	{
		cusp::multiply(CondTrue, s, temp_vec);
		CFA::AccumVec(r_prime, temp_vec);
		cusp::multiply(CondFalse, s, temp_vec);
		CFA::AccumVec(r_prime, temp_vec);
	}
	else if(tb)
	{
		cusp::multiply(CondTrue, s, temp_vec);
		CFA::AccumVec(r_prime, temp_vec);
	}
	else if(fb)
	{
		cusp::multiply(CondFalse, s, temp_vec);
		CFA::AccumVec(r_prime, temp_vec);
	}
}

//entry point
template <>
void CFA<int, cusp::device_memory>::f_if()
{
	fprintf(stdout, "f_if\n");
	thrust::fill(s.begin(), s.end(), 0);
	AND_OP <<<GRID, BLOCK>>> (	TPC(&r[0]), 
								TPC(&IF[0]), 
								TPC(&s[0]), 
								int(r.size()));
	
	f_if_host(s);
}

//F_primNum
template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::f_primNum_device(const cusp::array1d<INDEX_TYPE, cusp::device_memory> &s)
{
	mat_info info1, info2, info3;
	get_ellmatrix_info(Fun, info1);
	column_select<INDEX_TYPE> <<<GRID, BLOCK>>> (	info1.num_rows,
													info1.num_cols,
													info1.num_cols_per_row,
													info1.pitch,
													TPC(&Fun.column_indices.values[0]), 
													TPC(&s[0]),
													TPC(&temp_Fun.column_indices.values[0]), 
													TPC(&temp_Fun.values.values[0]));
	cusp::multiply(sigma_prime, temp_Fun, vf_mat);
	cusp::multiply(Var[0], vf_mat, a_var_mat);

	//sigma + (a_var (X) NUM)
	get_ellmatrix_info(a_var_mat, info1);
	AccumMat <<<GRID, BLOCK>>> (info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info1.pitch,
								TPC(&a_var_mat.column_indices.values[0]),
								TPC(&temp_vec[0]));

	get_ellmatrix_info(temp_Mat[0], info1);
	OuterProduct <<<GRID, BLOCK>>> (TPC(&NUM_vec[0]),
									TPC(&temp_vec[0]),
									int(BOOL_vec.size()),
									int(temp_vec.size()),
									info1.num_rows,
									info1.num_cols,
									info1.num_cols_per_row,
									info1.pitch, 
									TPC(&temp_Mat[0].column_indices.values[0]),
									TPC(&temp_Mat[0].values.values[0]));

	get_ellmatrix_info(temp_Mat[0], info1);
	get_ellmatrix_info(sigma_prime, info2);
	get_ellmatrix_info(temp_Mat[1], info3);
	ell_add <<<GRID, BLOCK>>> (	info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info2.num_cols_per_row,
								info3.num_cols_per_row,
								info1.pitch,
								info2.pitch,
								info3.pitch,
								TPC(&temp_Mat[0].column_indices.values[0]),
								TPC(&sigma_prime.column_indices.values[0]),
								TPC(&temp_Mat[1].column_indices.values[0]),
								TPC(&temp_Mat[1].values.values[0]));
	sigma_prime = temp_Mat[1];

	//r_prime
	cusp::multiply(Body, vf_mat, temp_Body);
	get_ellmatrix_info(temp_Body, info1);
	AccumMat <<<GRID, BLOCK>>> (info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info1.pitch,
								TPC(&temp_Body.column_indices.values[0]),
								TPC(&r_prime[0]));
}

//entry point
template <>
void CFA<int, cusp::device_memory>::f_primNum()
{
	fprintf(stdout, "f_primBool\n");
	thrust::fill(s.begin(), s.end(), 0);
	AND_OP <<<GRID, BLOCK>>> (	TPC(&r[0]), 
								TPC(&PrimNum[0]), 
								TPC(&s[0]), 
								int(r.size()));
	
	f_primNum_host(s);
}

//F_primBool
template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::f_primBool_device(const cusp::array1d<INDEX_TYPE, cusp::device_memory> &s)
{
	mat_info info1, info2, info3;
	get_ellmatrix_info(Fun, info1);
	column_select<INDEX_TYPE> <<<GRID, BLOCK>>> (	info1.num_rows,
													info1.num_cols,
													info1.num_cols_per_row,
													info1.pitch,
													TPC(&Fun.column_indices.values[0]), 
													TPC(&s[0]),
													TPC(&temp_Fun.column_indices.values[0]), 
													TPC(&temp_Fun.values.values[0]));
	cusp::multiply(sigma_prime, temp_Fun, vf_mat);
	cusp::multiply(Var[0], vf_mat, a_var_mat);

	//sigma + (a_var (X) #T#F)
	get_ellmatrix_info(a_var_mat, info1);
	AccumMat <<<GRID, BLOCK>>> (info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info1.pitch,
								TPC(&a_var_mat.column_indices.values[0]),
								TPC(&temp_vec[0]));

	get_ellmatrix_info(temp_Mat[0], info1);
	OuterProduct <<<GRID, BLOCK>>> (TPC(&BOOL_vec[0]),
									TPC(&temp_vec[0]),
									int(BOOL_vec.size()),
									int(temp_vec.size()),
									info1.num_rows,
									info1.num_cols,
									info1.num_cols_per_row,
									info1.pitch, 
									TPC(&temp_Mat[0].column_indices.values[0]),
									TPC(&temp_Mat[0].values.values[0]));

	get_ellmatrix_info(temp_Mat[0], info1);
	get_ellmatrix_info(sigma_prime, info2);
	get_ellmatrix_info(temp_Mat[1], info3);
	ell_add <<<GRID, BLOCK>>> (	info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info2.num_cols_per_row,
								info3.num_cols_per_row,
								info1.pitch,
								info2.pitch,
								info3.pitch,
								TPC(&temp_Mat[0].column_indices.values[0]),
								TPC(&sigma_prime.column_indices.values[0]),
								TPC(&temp_Mat[1].column_indices.values[0]),
								TPC(&temp_Mat[1].values.values[0]));
	sigma_prime = temp_Mat[1];

	//r_prime
	cusp::multiply(Body, vf_mat, temp_Body);
	get_ellmatrix_info(temp_Body, info1);
	AccumMat <<<GRID, BLOCK>>> (info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info1.pitch,
								TPC(&temp_Body.column_indices.values[0]),
								TPC(&r_prime[0]));
}

//entry point
template <>
void CFA<int, cusp::device_memory>::f_primBool()
{
	fprintf(stdout, "f_primBool\n");
	thrust::fill(s.begin(), s.end(), 0);
	AND_OP <<<GRID, BLOCK>>> (	TPC(&r[0]), 
								TPC(&PrimBool[0]), 
								TPC(&s[0]), 
								int(r.size()));
	
	f_primBool_host(s);
}

//F_primVoid
template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::f_primVoid_device(const cusp::array1d<INDEX_TYPE, cusp::device_memory> &s)
{
	mat_info info1, info2, info3;
	get_ellmatrix_info(Fun, info1);
	column_select<INDEX_TYPE> <<<GRID, BLOCK>>> (	info1.num_rows,
													info1.num_cols,
													info1.num_cols_per_row,
													info1.pitch,
													TPC(&Fun.column_indices.values[0]), 
													TPC(&s[0]),
													TPC(&temp_Fun.column_indices.values[0]), 
													TPC(&temp_Fun.values.values[0]));
	cusp::multiply(sigma_prime, temp_Fun, vf_mat);
	cusp::multiply(Var[0], vf_mat, a_var_mat);

	//sigma + (a_var (X) VOID)
	get_ellmatrix_info(a_var_mat, info1);
	AccumMat <<<GRID, BLOCK>>> (info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info1.pitch,
								TPC(&a_var_mat.column_indices.values[0]),
								TPC(&temp_vec[0]));

	get_ellmatrix_info(temp_Mat[0], info1);
	OuterProduct <<<GRID, BLOCK>>> (TPC(&VOID_vec[0]),
									TPC(&temp_vec[0]),
									int(VOID_vec.size()),
									int(temp_vec.size()),
									info1.num_rows,
									info1.num_cols,
									info1.num_cols_per_row,
									info1.pitch, 
									TPC(&temp_Mat[0].column_indices.values[0]),
									TPC(&temp_Mat[0].values.values[0]));

	get_ellmatrix_info(temp_Mat[0], info1);
	get_ellmatrix_info(sigma_prime, info2);
	get_ellmatrix_info(temp_Mat[1], info3);
	ell_add <<<GRID, BLOCK>>> (	info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info2.num_cols_per_row,
								info3.num_cols_per_row,
								info1.pitch,
								info2.pitch,
								info3.pitch,
								TPC(&temp_Mat[0].column_indices.values[0]),
								TPC(&sigma_prime.column_indices.values[0]),
								TPC(&temp_Mat[1].column_indices.values[0]),
								TPC(&temp_Mat[1].values.values[0]));
	sigma_prime = temp_Mat[1];

	//r_prime
	cusp::multiply(Body, vf_mat, temp_Body);
	get_ellmatrix_info(temp_Body, info1);
	AccumMat <<<GRID, BLOCK>>> (info1.num_rows,
								info1.num_cols,
								info1.num_cols_per_row,
								info1.pitch,
								TPC(&temp_Body.column_indices.values[0]),
								TPC(&r_prime[0]));
}

// //entry point
template <>
void CFA<int, cusp::device_memory>::f_primVoid()
{
	fprintf(stdout, "f_PrimVoid\n");
	thrust::fill(s.begin(), s.end(), 0);
	AND_OP <<<GRID, BLOCK>>> (	TPC(&r[0]), 
								TPC(&PrimVoid[0]), 
								TPC(&s[0]), 
								int(r.size()));

	f_primVoid_device(s);
}

#endif