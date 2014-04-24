#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "primitives_device.h"

/////////////////////////////////////////////////////////////////////////
/////////////////  Entry Wrapper Functions  /////////////////////////////
/////////////////////////////////////////////////////////////////////////
template <typename VALUE_TYPE>
void FILL(	cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
			const VALUE_TYPE value)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	FILL<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			TPC(&a[0]),
			value,
			int(a.size()));
}

template <typename VALUE_TYPE>
void FILL(	cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
			const VALUE_TYPE value,
			cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	FILL<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			TPC(&a[0]),
			value,
			int(a.size()));
}

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
void count(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
			const VALUE_TYPE val,
			INDEX_TYPE *h_res,
			INDEX_TYPE *d_res,
			cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = 1;
	const size_t BLOCK_SIZE = 512;

	count<INDEX_TYPE, VALUE_TYPE, 512> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			TPC(&a[0]),
			val,
			d_res,
			int(a.size()));

	cudaMemcpyAsync(h_res, d_res, sizeof(INDEX_TYPE), cudaMemcpyDeviceToHost, stream);
}

template <typename VALUE_TYPE>
void gather_reduce(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &indices,
					const int index)
{
	const size_t NUM_BLOCKS = 1;				//1 BLOCK for 
	const size_t BLOCK_SIZE = BLOCK_THREADS;

#if DEBUG
	assert(a.size() == b.size());
#endif

	gather_reduce<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&indices[index]),
			int(a.size()));
}

template <typename VALUE_TYPE>
void gather_reduce(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &indices,
					const int index,
					cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = 1;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

#if DEBUG
	assert(a.size() == b.size());
#endif

	gather_reduce<VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&indices[index]),
			int(a.size()));
}

// template <typename INDEX_TYPE, typename VALUE_TYPE>
// void column_select(	const cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
// 					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
// 					const INDEX_TYPE index,
// 					cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
// {
// 	mat_info<INDEX_TYPE> infoA;
// 	get_ell_matrix_info<VALUE_TYPE> (A, infoA);

// #if(DEBUG)
// 	assert(infoA.num_cols == s.size());
// 	assert(infoA.num_rows == y.size());
// #endif

// 	const size_t NUM_BLOCKS = BLOCKS;
// 	const size_t BLOCK_SIZE = BLOCK_THREADS;

// 	column_select_ell<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
// 			infoA.num_rows,
// 			infoA.num_cols,
// 			infoA.num_cols_per_row,
// 			infoA.pitch,
// 			TPC(&A.column_indices.values[0]),
// 			TPC(&s[index]),
// 			TPC(&y[0]));
// }

// template <typename INDEX_TYPE, typename VALUE_TYPE>
// void column_select(	const cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
// 					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
// 					const INDEX_TYPE index,
// 					cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
// 					cudaStream_t &stream)
// {
// 	mat_info<INDEX_TYPE> infoA;
// 	get_ell_matrix_info<VALUE_TYPE> (A, infoA);

// #if(DEBUG)
// 	assert(infoA.num_cols == s.size());
// 	assert(infoA.num_rows == y.size());
// #endif

// 	const size_t NUM_BLOCKS = BLOCKS;
// 	const size_t BLOCK_SIZE = BLOCK_THREADS;

// 	column_select_ell<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
// 			infoA.num_rows,
// 			infoA.num_cols,
// 			infoA.num_cols_per_row,
// 			infoA.pitch,
// 			TPC(&A.column_indices.values[0]),
// 			TPC(&s[index]),
// 			TPC(&y[0]));
// }

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
					const INDEX_TYPE index,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
{
	mat_info<INDEX_TYPE> infoA;
	get_csr_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == s.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	column_select<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			infoA.num_rows,
			TPC(&A.row_offsets[0]),
			TPC(&A.column_indices[0]),
			TPC(&s[index]),
			TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
					const INDEX_TYPE index,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
					cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_csr_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == s.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	column_select<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			infoA.num_rows,
			TPC(&A.row_offsets[0]),
			TPC(&A.column_indices[0]),
			TPC(&s[index]),
			TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select_if(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &cond,
						const INDEX_TYPE index,
						cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
{
	mat_info<INDEX_TYPE> infoA;
	get_csr_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == s.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	column_select_if<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			infoA.num_rows,
			TPC(&A.row_offsets[0]),
			TPC(&A.column_indices[0]),
			TPC(&s[index]),
			TPC(&cond[index]),
			TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select_if(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &cond,
						const INDEX_TYPE index,
						cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
						cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_csr_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == s.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	column_select_if<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			infoA.num_rows,
			TPC(&A.row_offsets[0]),
			TPC(&A.column_indices[0]),
			TPC(&s[index]),
			TPC(&cond[index]),
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
	mat_info<INDEX_TYPE> info;
	get_ell_matrix_info<VALUE_TYPE> (mat, info);

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
	mat_info<INDEX_TYPE> info;
	get_ell_matrix_info<VALUE_TYPE> (mat, info);

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
	mat_info<INDEX_TYPE> info;
	get_ell_matrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProduct<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			info.num_rows,
			info.num_cols,
			info.num_cols_per_row,
			info.pitch,
			TPC(&mat.column_indices.values[0]),
			TPC(&mat.values.values[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void OuterProductAdd(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &index_count,
						cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat)
{
	mat_info<INDEX_TYPE> info;
	get_ell_matrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProductAdd<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&index_count[0]),
			info.num_rows,
			info.num_cols,
			info.num_cols_per_row,
			info.pitch,
			TPC(&mat.column_indices.values[0]),
			TPC(&mat.values.values[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void OuterProductAdd(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &index_count,
						cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat,
						cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> info;
	get_ell_matrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProductAdd<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&index_count[0]),
			info.num_rows,
			info.num_cols,
			info.num_cols_per_row,
			info.pitch,
			TPC(&mat.column_indices.values[0]),
			TPC(&mat.values.values[0]));
}

template <typename VALUE_TYPE>
void OuterProductAdd(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &index_count,
						shared_store &mat,
						cudaStream_t &stream)
{
#if(DEBUG)
	assert(mat.num_rows == a.size());
	assert(mat.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProductAdd<int, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (	
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&index_count[0]),
			mat.num_rows,
			mat.num_cols,
			mat.num_cols_per_row,
			mat.pitch,
			mat.column_indices,
			mat.values);
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_add(	cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &B,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &C)
{
	mat_info<INDEX_TYPE> infoA, infoB, infoC;

	get_ell_matrix_info<VALUE_TYPE> (A, infoA);
	get_ell_matrix_info<VALUE_TYPE> (B, infoB);
	get_ell_matrix_info<VALUE_TYPE> (C, infoC);

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
	mat_info<INDEX_TYPE> infoA, infoB, infoC;

	get_ell_matrix_info<VALUE_TYPE> (A, infoA);
	get_ell_matrix_info<VALUE_TYPE> (B, infoB);
	get_ell_matrix_info<VALUE_TYPE> (C, infoC);

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
void ell_spmv(	const cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
{
	mat_info<INDEX_TYPE> infoA;
	get_ell_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_ellb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			infoA.num_rows,
            infoA.num_cols_per_row,
            infoA.pitch,
            TPC(&A.column_indices.values[0]),
            TPC(&x[0]), 
        	TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_spmv(	const cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
				cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_ell_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_ellb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			infoA.num_rows,
            infoA.num_cols_per_row,
            infoA.pitch,
            TPC(&A.column_indices.values[0]),
            TPC(&x[0]), 
        	TPC(&y[0]));
}

// template <typename INDEX_TYPE, typename VALUE_TYPE>
// void ell_spmv(	const shared_store &A,
//      			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
// 				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
// {
// #if(DEBUG)
// 	assert(A.num_cols == x.size());
// 	assert(A.num_rows == y.size());
// #endif

// 	const size_t NUM_BLOCKS = BLOCKS;
// 	const size_t BLOCK_SIZE = BLOCK_THREADS;

// 	spmv_ellb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
// 			A.num_rows,
//             A.num_cols_per_row,
//             A.pitch,
//             A.column_indices,
//             TPC(&x[0]), 
//         	TPC(&y[0]));
// }

template <typename VALUE_TYPE>
void ell_spmv(	const shared_store &A,
     			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
				cudaStream_t &stream)
{
#if(DEBUG)
	assert(A.num_cols == x.size());
	assert(A.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_ellb<int, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			A.num_rows,
            A.num_cols_per_row,
            A.pitch,
            A.column_indices,
            TPC(&x[0]), 
        	TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void csr_spmv(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y)
{
	mat_info<INDEX_TYPE> infoA;
	get_csr_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_csrb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			infoA.num_rows,
			TPC(&A.row_offsets[0]),
            TPC(&A.column_indices[0]),
            TPC(&x[0]), 
        	TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void csr_spmv(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     			const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
				cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
				cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_csr_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_csrb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			infoA.num_rows,
			TPC(&A.row_offsets[0]),
            TPC(&A.column_indices[0]),
            TPC(&x[0]), 
        	TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void LoadEllMatrix_device(	cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src,
							cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &dst)
{
	//src.sort_by_row_and_column();
	dst.resize(src.num_rows, src.num_cols, src.num_entries, std::max(src.num_cols/16, ulong(48)));

	mat_info<INDEX_TYPE> infoDst;
	get_ell_matrix_info<VALUE_TYPE> (dst, infoDst);

#if(DEBUG)
	assert(src.num_rows == infoDst.num_rows);
	assert(src.num_cols == infoDst.num_cols);
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	LoadEllMatrix<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			src.num_rows,
			src.num_entries,
			infoDst.num_cols_per_row,
			infoDst.pitch,
			TPC(&src.row_offsets[0]),
			TPC(&src.column_indices[0]),
			TPC(&src.values[0]),
			TPC(&dst.column_indices.values[0]),
			TPC(&dst.values.values[0]));

}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void LoadEllMatrix_device(	cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src,
							shared_store &dst)
{

#if(DEBUG)
	assert(src.num_rows == dst.num_rows);
	assert(src.num_cols == dst.num_cols);
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	LoadEllMatrix<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			src.num_rows,
			src.num_entries,
			dst.num_cols_per_row,
			dst.pitch,
			TPC(&src.row_offsets[0]),
			TPC(&src.column_indices[0]),
			TPC(&src.values[0]),
			dst.column_indices,
			dst.values);

}

#endif