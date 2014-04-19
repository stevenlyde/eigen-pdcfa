#ifndef PRIMITIVES_H
#define PRIMITIVES_H

typedef struct __mat_info
{
	int num_rows;
	int num_cols;
	int num_entries;
	int num_cols_per_row;
	int pitch;
} mat_info;

template <typename VALUE_TYPE>
inline void get_ellmatrix_info(	const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &mat, mat_info &info)
{
	info.num_rows = mat.num_rows;
	info.num_cols = mat.num_cols;
	info.num_entries = mat.num_entries;
	info.num_cols_per_row = mat.column_indices.num_cols;
	info.pitch = mat.column_indices.pitch;
}

template <typename VALUE_TYPE>
inline void print_matrix_info(cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &mat)
{
	mat_info info;
	get_ellmatrix_info(mat, info);
	fprintf(stderr, "rows: %d  cols: %d  num_entries: %d  num_cols_per_row: %d  pitch: %d\n", 
		info.num_rows, info.num_cols, info.num_entries, info.num_cols_per_row, info.pitch);
}

template <typename VALUE_TYPE>
inline __device__ void reduce_sum(VALUE_TYPE *a, const int size, const int tID);

template <typename VALUE_TYPE>
inline __device__ void reduce_sum(VALUE_TYPE *a, const int size, const int tID)
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

template <typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
AND_OP(	const VALUE_TYPE *A,
		const VALUE_TYPE *B,
		VALUE_TYPE *C,
		const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	for(int i=tID; i<size; i+=grid_size)
		C[i] = A[i] & B[i];
}

template <typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
get_indices(	const VALUE_TYPE *a,
				VALUE_TYPE *b,
				const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	__shared__ int index;
	index = 0;
	__syncthreads();

	for(int i=tID; i<size; i+=grid_size)
	{
		if(a[i] != 0)
		{
			int old_index = atomicAdd(&index, 1);
			b[old_index] = i;
		}
	}
}

template <typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void AccumVec(	VALUE_TYPE *a,
							const VALUE_TYPE *b,
							const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	//a += b
	for(int i=tID; i<size; i+=grid_size)
		a[i] += b[i];
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void AccumMat(	const 	INDEX_TYPE num_rows,
							const 	INDEX_TYPE num_cols,
							const 	INDEX_TYPE num_cols_per_row,
							const 	INDEX_TYPE pitch,
							const 	INDEX_TYPE *column_indices,
									VALUE_TYPE *a)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory>::invalid_index;

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

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
column_select(	const 	INDEX_TYPE num_rows,
				const 	INDEX_TYPE num_cols,
				const 	INDEX_TYPE num_cols_per_row,
				const 	INDEX_TYPE pitch,
				const	INDEX_TYPE *A_column_indices,
				const 	VALUE_TYPE *s,
						VALUE_TYPE *y)
{
	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory>::invalid_index;

	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	const int index = s[0];

	for(int row=tID; row < num_rows; row+=grid_size)
	{
		y[row] = 0;
		INDEX_TYPE offset = row;
		for(int n=0; n < num_cols_per_row; ++n)
		{
			int col = A_column_indices[offset];
			if(col != invalid_index && col == index)
			{
				y[row] = 1;
				offset += pitch;
			}
		}
	}
}

template <typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
InnerProductStore(	const VALUE_TYPE *a,
					const VALUE_TYPE *b,
					const int size,
					VALUE_TYPE *c)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	for(int index=tID; index < size; index+=grid_size)
	{
		if(a[index] != 0 && b[index] != 0)
		{
			c[0] = 1;
		}
	}
}

//a (.) Mat -> b
template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void InnerProduct(	const VALUE_TYPE *a,
								VALUE_TYPE *b,
								const INDEX_TYPE size_a,
								const INDEX_TYPE size_b,
								const INDEX_TYPE num_rows,
								const INDEX_TYPE num_cols,
								const INDEX_TYPE num_cols_per_row,
								const INDEX_TYPE pitch,
								const INDEX_TYPE *column_indices)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;
	
	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory>::invalid_index;

	for(int row=tID; row<size_a; row+=grid_size)
	{
		int offset = row;
		for(int n=0; n < num_cols_per_row; ++n, offset+=pitch)
		{
			int col = column_indices[offset];
			if(col != invalid_index)
			{
				if(a[row] == 1)
					b[col] = 1;
			}
		}
	}
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void OuterProduct(	const VALUE_TYPE *a,
								const VALUE_TYPE *b,
								const INDEX_TYPE size_a,
								const INDEX_TYPE size_b,
								const INDEX_TYPE num_rows,
								const INDEX_TYPE num_cols,
								const INDEX_TYPE num_cols_per_row,
								const INDEX_TYPE pitch,
								INDEX_TYPE *column_indices,
								VALUE_TYPE *values)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;
	
	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

	__shared__ int entries_b[BLOCK_THREADS];
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

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void
spmv_ellb(	const INDEX_TYPE num_rows, 
            const INDEX_TYPE num_cols, 
            const INDEX_TYPE num_cols_per_row,
            const INDEX_TYPE pitch,
            const INDEX_TYPE * Aj,
            const VALUE_TYPE * x, 
                  VALUE_TYPE * y)
{
    const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

    const INDEX_TYPE thread_id = blockDim.x * blockIdx.x + threadIdx.x;
    const INDEX_TYPE grid_size = gridDim.x * blockDim.x;

    for(INDEX_TYPE row = thread_id; row < num_rows; row += grid_size)
    {
    	VALUE_TYPE sum = 0;
        INDEX_TYPE offset = row;
        for(INDEX_TYPE n = 0; n < num_cols_per_row; n++)
        {
            const INDEX_TYPE col = Aj[offset];
            if(col != invalid_index)
            {
	            if(x[col] != 0)
	            	sum = 1;
            }

            offset += pitch;
        }

        y[row] = sum;
	}
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
ell_add(	const INDEX_TYPE num_rows,
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
			VALUE_TYPE *C_values)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	//__shared__ int entry_count[BLOCK_THREADS];
	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory>::invalid_index;

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

	// reduce_sum<INDEX_TYPE>(entry_count, BLOCK_THREADS, tID);
	// if(tID == 0)
	// 	*C_num_entries = entry_count[0];
}

/////////////////////////////////////////////////////////////////////////
/////////////////  Entry Wrapper Functions  /////////////////////////////
/////////////////////////////////////////////////////////////////////////
template <typename VALUE_TYPE>
void AND_OP(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
			cusp::array1d<VALUE_TYPE, cusp::device_memory> &c)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	AND_OP<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&c[0]),
			int(a.size()));
}

template <typename VALUE_TYPE>
void AND_OP(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
			cusp::array1d<VALUE_TYPE, cusp::device_memory> &c,
			cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	AND_OP<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&c[0]),
			int(a.size()));
}

template <typename VALUE_TYPE>
void get_indices(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &b)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	get_indices<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			int(a.size()));
}

template <typename VALUE_TYPE>
void get_indices(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
					cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	get_indices<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			int(a.size()));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select(	const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &A,
					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
					const INDEX_TYPE index,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
{
	mat_info infoA;
	get_ellmatrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == s.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	column_select<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			infoA.num_rows,
			infoA.num_cols,
			infoA.num_cols_per_row,
			infoA.pitch,
			TPC(&A.column_indices.values[0]),
			TPC(&s[index]),
			TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select(	const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &A,
					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
					const INDEX_TYPE index,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
					cudaStream_t &stream)
{
	mat_info infoA;
	get_ellmatrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == s.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	column_select<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			infoA.num_rows,
			infoA.num_cols,
			infoA.num_cols_per_row,
			infoA.pitch,
			TPC(&A.column_indices.values[0]),
			TPC(&s[index]),
			TPC(&y[0]));
}

template <typename VALUE_TYPE>
void AccumVec(	cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
				const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	AccumVec<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			int(a.size()));
}

template <typename VALUE_TYPE>
void AccumVec(	cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
				const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
				cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	AccumVec<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			int(a.size()));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void AccumMat(	const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &mat,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &vec,
				cudaStream_t &stream)
{
	mat_info info;
	get_ellmatrix_info<VALUE_TYPE> (mat, info);

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	AccumMat<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			info.num_rows,
			info.num_cols,
			info.num_cols_per_row,
			info.pitch,
			TPC(&mat.column_indices.values[0]),
			TPC(&vec[0]));
}

template <typename VALUE_TYPE>
void InnerProductStore(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
						cusp::array1d<VALUE_TYPE, cusp::device_memory> &c,
						const int index)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	InnerProductStore<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			int(a.size()),
			TPC(&c[index]));
}

template <typename VALUE_TYPE>
void InnerProductStore(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
						cusp::array1d<VALUE_TYPE, cusp::device_memory> &c,
						const int index,
						cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	InnerProductStore<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			int(a.size()),
			TPC(&c[index]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void OuterProduct(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
					cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat)
{
	mat_info info;
	get_ellmatrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProduct<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			INDEX_TYPE(a.size()),
			INDEX_TYPE(b.size()),
			info.num_rows,
			info.num_cols,
			info.num_cols_per_row,
			info.pitch,
			TPC(&mat.column_indices.values[0]),
			TPC(&mat.values.values[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void OuterProduct(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
					cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat,
					cudaStream_t &stream)
{
	mat_info info;
	get_ellmatrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProduct<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			INDEX_TYPE(a.size()),
			INDEX_TYPE(b.size()),
			info.num_rows,
			info.num_cols,
			info.num_cols_per_row,
			info.pitch,
			TPC(&mat.column_indices.values[0]),
			TPC(&mat.values.values[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_add(	cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &B,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &C)
{
	mat_info infoA, infoB, infoC;

	get_ellmatrix_info<VALUE_TYPE> (A, infoA);
	get_ellmatrix_info<VALUE_TYPE> (B, infoB);
	get_ellmatrix_info<VALUE_TYPE> (C, infoC);

#if(DEBUG)
	assert(infoA.num_rows == infoB.num_rows && infoA.num_rows == infoC.num_rows);
	assert(infoA.num_cols == infoB.num_cols && infoA.num_cols == infoC.num_cols);
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;
	
	ell_add<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			infoA.num_rows,
			infoA.num_cols,
			infoA.num_cols_per_row,
			infoB.num_cols_per_row,
			infoC.num_cols_per_row,
			infoA.pitch,
			infoB.pitch,
			infoC.pitch,
			TPC(&A.column_indices.values[0]),
			TPC(&B.column_indices.values[0]),
			TPC(&C.column_indices.values[0]),
			TPC(&C.values.values[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_add(	cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &B,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &C,
				cudaStream_t &stream)
{
	mat_info infoA, infoB, infoC;

	get_ellmatrix_info<VALUE_TYPE> (A, infoA);
	get_ellmatrix_info<VALUE_TYPE> (B, infoB);
	get_ellmatrix_info<VALUE_TYPE> (C, infoC);

#if(DEBUG)
	assert(infoA.num_rows == infoB.num_rows && infoA.num_rows == infoC.num_rows);
	assert(infoA.num_cols == infoB.num_cols && infoA.num_cols == infoC.num_cols);
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;
	
	ell_add<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			infoA.num_rows,
			infoA.num_cols,
			infoA.num_cols_per_row,
			infoB.num_cols_per_row,
			infoC.num_cols_per_row,
			infoA.pitch,
			infoB.pitch,
			infoC.pitch,
			TPC(&A.column_indices.values[0]),
			TPC(&B.column_indices.values[0]),
			TPC(&C.column_indices.values[0]),
			TPC(&C.values.values[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_spmv(	const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &A,
     			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
{
	mat_info infoA;
	get_ellmatrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_ellb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			infoA.num_rows, 
            infoA.num_cols, 
            infoA.num_cols_per_row,
            infoA.pitch,
            TPC(&A.column_indices.values[0]),
            TPC(&x[0]), 
        	TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_spmv(	const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &A,
     			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
				cudaStream_t &stream)
{
	mat_info infoA;
	get_ellmatrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_ellb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			infoA.num_rows, 
            infoA.num_cols, 
            infoA.num_cols_per_row,
            infoA.pitch,
            TPC(&A.column_indices.values[0]),
            TPC(&x[0]), 
        	TPC(&y[0]));
}

#endif