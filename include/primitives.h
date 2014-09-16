#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "primitives_device.h"
#include "matrix_ops_device.h"
#include "scan.h"

/////////////////////////////////////////////////////////////////////////
/////////////////  Entry Wrapper Functions  /////////////////////////////
/////////////////////////////////////////////////////////////////////////
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
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
					cudaStream_t &stream)
{
	const size_t NUM_BLOCKS = 1;
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

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
					const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
					const INDEX_TYPE index,
					cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
					cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_matrix_info<VALUE_TYPE> (A, infoA);

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
						cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
						cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_matrix_info<VALUE_TYPE> (A, infoA);

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
					cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat,
					cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> info;
	get_matrix_info<VALUE_TYPE> (mat, info);

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
						cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat,
						cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> info;
	get_matrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProductAdd_ELL<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
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
						hyb_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat,
						cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> info;
	get_matrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProductAdd_HYB<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&index_count[0]),
			info.num_rows,
			info.num_cols,
			info.num_cols_per_row,
			info.pitch,
			TPC(&mat.row_sizes[0]),
			TPC(&mat.matrix.ell.column_indices.values[0]),
			TPC(&mat.matrix.coo.row_indices[0]),
			TPC(&mat.matrix.coo.column_indices[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void OuterProductAdd(	const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
						const cusp::array1d<VALUE_TYPE, cusp::device_memory> &index_count,
						dell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat,
						cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> info;
	get_matrix_info<VALUE_TYPE> (mat, info);

#if(DEBUG)
	assert(info.num_rows == a.size());
	assert(info.num_cols == b.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	OuterProductAdd_DELL<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			TPC(&a[0]),
			TPC(&b[0]),
			TPC(&index_count[0]),
			TPC(&(*mat.row_offsets)[0]),
			TPC(&(*mat.column_indices)[0]),
			TPC(&mat.row_sizes[0]),
			TPC(&mat.coo.row_indices[0]),
			TPC(&mat.coo.column_indices[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_add(	cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &B,
				cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &C,
				cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA, infoB, infoC;

	get_matrix_info<VALUE_TYPE> (A, infoA);
	get_matrix_info<VALUE_TYPE> (B, infoB);
	get_matrix_info<VALUE_TYPE> (C, infoC);

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
void spmv(	const cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     		const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
			cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
			cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_matrix_info<VALUE_TYPE> (A, infoA);

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

template <typename INDEX_TYPE, typename VALUE_TYPE>
void spmv(	const hyb_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     		const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
			cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
			cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_hybb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			infoA.num_rows,
    	    infoA.num_cols_per_row,
   			infoA.pitch,
        	TPC(&A.matrix.ell.column_indices.values[0]),
        	TPC(&A.matrix.coo.row_indices[0]),
        	TPC(&A.matrix.coo.column_indices[0]),
        	TPC(&x[0]), 
    		TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void spmv(	const cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     		const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
			cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
			cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_matrix_info<VALUE_TYPE> (A, infoA);

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
void spmv(	const dell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
     		const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
			cusp::array1d<VALUE_TYPE, cusp::device_memory> &y,
			cudaStream_t &stream)
{
	mat_info<INDEX_TYPE> infoA;
	get_matrix_info<VALUE_TYPE> (A, infoA);

#if(DEBUG)
	assert(infoA.num_cols == x.size());
	assert(infoA.num_rows == y.size());
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	spmv_dellb<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE, 0, stream>>> (
			infoA.num_rows,
			TPC(&(*A.row_offsets)[0]),
            TPC(&(*A.column_indices)[0]),
            TPC(&A.coo.row_indices[0]),
            TPC(&A.coo.column_indices[0]),
            TPC(&x[0]), 
        	TPC(&y[0]));
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void LoadEllMatrix_device(	cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src,
							cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &dst)
{
	//src.sort_by_row_and_column();
	dst.resize(src.num_rows, src.num_cols, src.num_entries, std::max(src.num_cols/16, ulong(64)));

	mat_info<INDEX_TYPE> infoDst;
	get_matrix_info<VALUE_TYPE> (dst, infoDst);

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
void LoadHybMatrix_device(	cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src,
							hyb_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &dst)
{
	//src.sort_by_row_and_column();
	const INDEX_TYPE invalid_index = -1;
	dst.resize(src.num_rows, src.num_cols, src.num_entries, 256, std::max(src.num_cols/16, ulong(96)));
	thrust::fill(dst.matrix.ell.column_indices.values.begin(), dst.matrix.ell.column_indices.values.end(), invalid_index);

	mat_info<INDEX_TYPE> infoDst;
	get_matrix_info<VALUE_TYPE> (dst, infoDst);

#if(DEBUG)
	assert(src.num_rows == infoDst.num_rows);
	assert(src.num_cols == infoDst.num_cols);
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	fprintf(stderr, "num_cols_per_row: %d\n", infoDst.num_cols_per_row);

	LoadHybMatrix<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			src.num_rows,
			src.num_entries,
			infoDst.num_cols_per_row,
			infoDst.pitch,
			TPC(&src.row_offsets[0]),
			TPC(&src.column_indices[0]),
			TPC(&dst.row_sizes[0]),
			TPC(&dst.matrix.ell.column_indices.values[0]),
			TPC(&dst.matrix.coo.row_indices[0]),
			TPC(&dst.matrix.coo.column_indices[0]));

	dst.matrix.num_entries = src.num_entries;
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void LoadDellMatrix_device(	cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src,
							dell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &dst)
{
	#define ROW_SIZE	1024
	dst.resize(src.num_rows, src.num_cols, src.num_entries, src.num_rows*ROW_SIZE, 4096);

	mat_info<INDEX_TYPE> infoDst;
	get_matrix_info<VALUE_TYPE> (dst, infoDst);

#if(DEBUG)
	assert(src.num_rows == infoDst.num_rows);
	assert(src.num_cols == infoDst.num_cols);
#endif

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	LoadDellMatrix<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			src.num_rows,
			ROW_SIZE,
			TPC(&src.row_offsets[0]),
			TPC(&src.column_indices[0]),
			TPC(&(*dst.row_offsets)[0]),
			TPC(&(*dst.column_indices)[0]),
			TPC(&dst.row_sizes[0]),
			TPC(&dst.coo.row_indices[0]),
			TPC(&dst.coo.column_indices[0]));

	dst.num_entries = src.num_entries;
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void RebuildDellMatrix_device(	dell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src )
{
	mat_info<INDEX_TYPE> infoSrc;
	get_matrix_info<VALUE_TYPE> (src, infoSrc);

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	cusp::array1d<INDEX_TYPE, cusp::device_memory> *new_column_indices;
	cusp::array1d<INDEX_TYPE, cusp::device_memory> *new_row_offsets;
	if(src.column_indices == &src.column_indicesA)
	{
		new_column_indices = &src.column_indicesB;
		new_row_offsets = &src.row_offsetsB;
	}
	else
	{
		new_column_indices = &src.column_indicesA;
		new_row_offsets = &src.row_offsetsA;
	}

	CalcRowOffsets<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			infoSrc.num_rows,
			TPC(&src.row_offsets[0]),
			TPC(&src.column_indices[0]));

	thrust::inclusive_scan((*new_row_offsets).begin(), (*new_row_offsets).end(), (*new_row_offsets).begin());

	RebuildDellMatrix<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			infoSrc.num_rows,
			TPC(&(*src.row_offsets)[0]),
			TPC(&(*new_row_offsets)[0]),
			TPC(&(*src.column_indices)[0]),
			TPC(&(*new_column_indices)[0]));

	src.row_offsets = new_row_offsets;
	src.column_indices = new_column_indices;
}

template <typename INDEX_TYPE, typename VALUE_TYPE>
void RebuildMatrix(	cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src_CSR,
					cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &src_ELL,
					cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &dst)
{
	mat_info<INDEX_TYPE> infoSrcCSR, infoSrcELL;
	get_matrix_info<VALUE_TYPE> (src_CSR, infoSrcCSR);
	get_matrix_info<VALUE_TYPE> (src_ELL, infoSrcELL);

	const size_t NUM_BLOCKS = BLOCKS;
	const size_t BLOCK_SIZE = BLOCK_THREADS;

	CalcRowOffsets<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			infoSrcCSR.num_rows,
			infoSrcELL.num_cols_per_row,
			infoSrcELL.pitch,
			TPC(&src_CSR.row_offsets[0]),
			TPC(&src_ELL.column_indices[0]),
			TPC(&dst.row_offsets[0]));

	thrust::inclusive_scan(dst.row_offsets.begin(), dst.row_offsets.end(), dst.row_offsets.begin());

	RebuildMatrix<INDEX_TYPE, VALUE_TYPE> <<<NUM_BLOCKS, BLOCK_SIZE>>> (
			infoSrcCSR.num_rows,
			infoSrcELL.num_cols_per_row,
			infoSrcELL.pitch,
			TPC(&src_CSR.row_offsets[0]),
			TPC(&src_CSR.column_indices[0]),
			TPC(&src_ELL.column_indices[0]),
			TPC(&dst.row_offsets[0]),
			TPC(&dst.column_indices[0]));
}

#endif
