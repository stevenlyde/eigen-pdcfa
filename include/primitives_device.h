#ifndef PRIMITIVES_DEVICE_H
#define PRIMITIVES_DEVICE_H

//shared memory size = BLOCK_SIZE
//shared memory reduce
template <typename VALUE_TYPE, int BLOCK_SIZE>
inline __device__ void reduce_sum(VALUE_TYPE *a, const int tID)
{
	int n = BLOCK_SIZE;
	while(n > 1)
	{
		int half = n / 2;
		if(tID < half)
			a[tID] += a[n - tID - 1];
		n = n - half;
		__syncthreads();
	}
}

template <typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
FILL(	VALUE_TYPE *a,
		const VALUE_TYPE value,
		const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	for(int i=tID; i<size; i+=grid_size)
		a[i] = value;
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

//b size = a size + 1
template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
gather_reduce(	const VALUE_TYPE *a,
				VALUE_TYPE *b,
				INDEX_TYPE *index_count,
				const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	__shared__ int count[BLOCK_THREADS];
	__shared__ int index;
	count[tID] = 0;
	index = 0;
	__syncthreads();

	for(int i=tID; i<size; i+=grid_size)
	{
		if(a[i] != 0)
		{
			int old_index = atomicAdd(&index, 1);
			b[old_index] = i;
			count[tID]++;
		}
	}
	__syncthreads();

	reduce_sum<VALUE_TYPE, BLOCK_THREADS>(count, tID);

	if(tID == 0)
		index_count[0] = count[0];
}

template <typename INDEX_TYPE, typename VALUE_TYPE, int BLOCK_SIZE>
__launch_bounds__(BLOCK_SIZE,1)
__global__ void 
count(	const VALUE_TYPE *a,
		const VALUE_TYPE val,
		INDEX_TYPE *res,
		const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	__shared__ INDEX_TYPE count[BLOCK_SIZE];
	count[tID] = 0;
	__syncthreads();

	for(int i=tID; i < size; i+=grid_size)
	{
		if(a[i] == val)
			count[i]++;
	}
	__syncthreads();

	reduce_sum<VALUE_TYPE, BLOCK_SIZE>(count, tID);

	if(tID == 0)
		res[0] = count[0];
}

template <typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
AccumVec(	VALUE_TYPE *a,
			const VALUE_TYPE *b,
			const int size)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	//a += b
	for(int i=tID; i<size; i+=grid_size)
	{
		if(b[i])
			a[i] = 1;
		//a[i] += b[i];
	}
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
column_select(	const INDEX_TYPE num_rows,
				const INDEX_TYPE *A_row_offsets,
				const INDEX_TYPE *A_column_indices,
				const VALUE_TYPE *s,
				VALUE_TYPE *y)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	const INDEX_TYPE index = s[0];

	for(int row=tID; row < num_rows; row+=grid_size)
	{
		INDEX_TYPE offset = A_row_offsets[row];
		INDEX_TYPE num_cols_per_row = A_row_offsets[row + 1] - offset;

		VALUE_TYPE val = 0;
		for(int n=0; n < num_cols_per_row; ++n, ++offset)
		{
			int col = A_column_indices[offset];
			if(col == index)
			{
				val = 1;
			}
		}

		y[row] = val;
	}
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
column_select_if(	const INDEX_TYPE num_rows,
					const INDEX_TYPE *A_row_offsets,
					const INDEX_TYPE *A_column_indices,
					const VALUE_TYPE *s,
					const VALUE_TYPE *cond,
					VALUE_TYPE *y)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;

	//check conditional value
	if(cond[0] == 0)
		return;

	const int index = s[0];

	for(int row=tID; row < num_rows; row+=grid_size)
	{
		INDEX_TYPE offset = A_row_offsets[row];
		INDEX_TYPE num_cols_per_row = A_row_offsets[row + 1] - offset;

		VALUE_TYPE val = 0;
		for(int n=0; n < num_cols_per_row; ++n, ++offset)
		{
			int col = A_column_indices[offset];
			if(col == index)
			{
				val = 1;
			}
		}

		y[row] = val;
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

		for(int i=0; i < num_rows; ++i)
		{
			if(a[i] != 0)
				num_entries_a++;
		}

		for(int i=0; i < num_cols; ++i)
		{
			if(b[i] != 0)
			{
				entries_b[num_entries_b] = i;
				num_entries_b++;
			}
		}
	}
	__syncthreads();

	for(int row=tID; row<num_rows; row+=grid_size)
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

//size of a must be num_rows + 1
//size of b must be num_cols + 1
//last entry of each array is used for storing entry count
//only add unique entries
template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void OuterProductAdd(	const VALUE_TYPE *a,
									const VALUE_TYPE *b,
									const INDEX_TYPE *index_count,
									const INDEX_TYPE num_rows,
									const INDEX_TYPE num_cols,
									const INDEX_TYPE num_cols_per_row,
									const INDEX_TYPE pitch,
									INDEX_TYPE *column_indices,
									VALUE_TYPE *values)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;
	const INDEX_TYPE invalid_index = cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory>::invalid_index;

	INDEX_TYPE num_entries_a = index_count[0];
	INDEX_TYPE num_entries_b = index_count[1];

	for(INDEX_TYPE j=tID; j < num_entries_a; j+=grid_size)
	{
		INDEX_TYPE row = a[j];
		for(INDEX_TYPE k=0; k < num_entries_b; ++k)
		{
			VALUE_TYPE b_col = b[k];
			INDEX_TYPE offset = row;
			for(INDEX_TYPE n=0; n < num_cols_per_row; ++n, offset+=pitch)
			{
				INDEX_TYPE col = column_indices[offset];
				if(col == b_col)
				{
					break;
				}
				else if(col == invalid_index)
				{
					column_indices[offset] = b_col;
					values[offset] = 1;
					break;
				}
			}
		}
	}
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void
spmv_ellb(	const INDEX_TYPE num_rows,
            const INDEX_TYPE num_cols_per_row,
            const INDEX_TYPE pitch,
            const INDEX_TYPE * Aj,
            const VALUE_TYPE * x, 
                  VALUE_TYPE * y)
{
    const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

    const INDEX_TYPE tID = blockDim.x * blockIdx.x + threadIdx.x;
    const INDEX_TYPE grid_size = gridDim.x * blockDim.x;

    for(INDEX_TYPE row=tID; row < num_rows; row += grid_size)
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
spmv_csrb(	const INDEX_TYPE num_rows,
            const INDEX_TYPE * A_row_offsets,
			const INDEX_TYPE * A_column_indices,
            const VALUE_TYPE * x, 
                  VALUE_TYPE * y)
{
    const INDEX_TYPE tID = blockDim.x * blockIdx.x + threadIdx.x;
    const INDEX_TYPE grid_size = gridDim.x * blockDim.x;

    for(INDEX_TYPE row=tID; row < num_rows; row += grid_size)
    {
    	INDEX_TYPE row_start = A_row_offsets[row];
    	INDEX_TYPE row_end = A_row_offsets[row + 1];

    	VALUE_TYPE sum = 0;
        for(INDEX_TYPE j=row_start; j < row_end; ++j)
        {
        	INDEX_TYPE col = A_column_indices[j];
        	if(x[col] != 0)
	    		sum = 1;
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
	}
	//__syncthreads();

	// reduce_sum<INDEX_TYPE>(entry_count, BLOCK_THREADS, tID);
	// if(tID == 0)
	// 	*C_num_entries = entry_count[0];
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
__launch_bounds__(BLOCK_THREADS,1)
__global__ void 
LoadEllMatrix(	const INDEX_TYPE num_rows,
				const INDEX_TYPE num_entries,
				const INDEX_TYPE num_cols_per_row,
				const INDEX_TYPE pitch,
				const INDEX_TYPE *src_row_offsets,
				const INDEX_TYPE *src_column_indices,
				const VALUE_TYPE *src_values,
				INDEX_TYPE *dst_column_indices,
				VALUE_TYPE *dst_values)
{
	const int tID = blockDim.x * blockIdx.x + threadIdx.x;
	const int grid_size = blockDim.x * gridDim.x;
	const INDEX_TYPE invalid_index = cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory>::invalid_index;

	for(int row=tID; row < num_rows; row+=grid_size)
	{
		INDEX_TYPE row_start = src_row_offsets[row];
		INDEX_TYPE row_end = src_row_offsets[row + 1];
		INDEX_TYPE offset = row;

		for(int j=row_start; j < row_end; ++j, offset+=pitch)
		{
			dst_column_indices[offset] = src_column_indices[j];
			dst_values[offset] = src_values[j];
		}

		while(offset < num_cols_per_row * pitch)
		{
			dst_column_indices[offset] = invalid_index;
			offset += pitch;
		}
	}
}

#endif