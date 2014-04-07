/*
 *  Copyright 2008-2009 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*! \file hybb_matrix.h
 *  \brief Hybrid ELL/COO matrix format
 */

#pragma once

#include <cusp/detail/config.h>

#include <cusp/array1d.h>
#include <cusp/format.h>
#include <cusp/detail/matrix_base.h>

namespace cusp
{
    // Forward definitions
    template <typename IndexType, class MemorySpace> class ellb_matrix;
    template <typename IndexType, class MemorySpace> class coob_matrix;
    template <typename Matrix1, typename Matrix2, typename IndexType, class MemorySpace> class hybb_matrix_view;

/*! \p hybb_matrix : Hybrid ELL/COO binary matrix container
*/

template <typename IndexType, class MemorySpace>
class hybb_matrix : public detail::binary_matrix_base<IndexType,MemorySpace,cusp::hybb_format>
{
  typedef cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::hybb_format> Parent;
    public:
    /*! rebind matrix to a different MemorySpace
     */
    template<typename MemorySpace2>
    struct rebind { typedef cusp::hybb_matrix<IndexType, MemorySpace2> type; };
    
    /*! equivalent container type
     */
    typedef typename cusp::hybb_matrix<IndexType, MemorySpace> container;

    /*! equivalent view type
     */
    typedef typename cusp::hybb_matrix_view<typename cusp::ellb_matrix<IndexType,MemorySpace>::view,
                                           typename cusp::coob_matrix<IndexType,MemorySpace>::view,
                                           IndexType, MemorySpace> view;
    
    /*! equivalent const_view type
     */
    typedef typename cusp::hybb_matrix_view<typename cusp::ellb_matrix<IndexType,MemorySpace>::const_view,
                                           typename cusp::coob_matrix<IndexType,MemorySpace>::const_view,
                                           IndexType, MemorySpace> const_view;
    
    /*! type of \p ELL portion of the HYB structure 
     */
    typedef cusp::ellb_matrix<IndexType,MemorySpace> ellb_matrix_type;
    
    /*! type of \p COO portion of the HYB structure 
     */
    typedef cusp::coob_matrix<IndexType,MemorySpace> coob_matrix_type;
    
    /*! Storage for the \p ellb_matrix portion.
     */
    ellb_matrix_type ell;
    
    /*! Storage for the \p ellb_matrix portion.
     */
    coob_matrix_type coo;

    /*! Construct an empty \p hybb_matrix.
     */
    hybb_matrix() {}

    /*! Construct a \p hybb_matrix with a specific shape and separation into ELL and COO portions.
     *
     *  \param num_rows Number of rows.
     *  \param num_cols Number of columns.
     *  \param num_ell_entries Number of nonzero matrix entries in the ELL portion.
     *  \param num_coo_entries Number of nonzero matrix entries in the ELL portion.
     *  \param num_entries_per_row Maximum number of nonzeros per row in the ELL portion.
     *  \param alignment Amount of padding used to align the ELL data structure (default 32).
     */
    hybb_matrix(IndexType num_rows, IndexType num_cols,
               IndexType num_ell_entries, IndexType num_coo_entries,
               IndexType num_entries_per_row, IndexType alignment = 32)
    : Parent(num_rows, num_cols, num_ell_entries + num_coo_entries),
      ell(num_rows, num_cols, num_ell_entries, num_entries_per_row, alignment),
      coo(num_rows, num_cols, num_coo_entries) {}

    // TODO remove default alignment of 32

    /*! Construct a \p hybb_matrix from another matrix.
     *
     *  \param matrix Another sparse or dense matrix.
     */
    template <typename MatrixType>
    hybb_matrix(const MatrixType& matrix);
    
    /*! Resize matrix dimensions and underlying storage
     */
    void resize(IndexType num_rows, IndexType num_cols,
                IndexType num_ell_entries, IndexType num_coo_entries,
                IndexType num_entries_per_row, IndexType alignment = 32)
    {
      Parent::resize(num_rows, num_cols, num_ell_entries + num_coo_entries);
      ell.resize(num_rows, num_cols, num_ell_entries, num_entries_per_row, alignment);
      coo.resize(num_rows, num_cols, num_coo_entries);
    }

    /*! Swap the contents of two \p hybb_matrix objects.
     *
     *  \param matrix Another \p hybb_matrix with the same IndexType and ValueType.
     */
    void swap(hybb_matrix& matrix)
    {
      Parent::swap(matrix);
      ell.swap(matrix.ell);
      coo.swap(matrix.coo);
    }
    
    /*! Assignment from another matrix.
     *
     *  \param matrix Another sparse or dense matrix.
     */
    template <typename MatrixType>
    hybb_matrix& operator=(const MatrixType& matrix);
}; // class hybb_matrix
/*! \}
 */


/*! \addtogroup sparse_matrix_views Sparse Matrix Views
 *  \ingroup sparse_matrices
 *  \{
 */

/*! \p hybb_matrix_view : Hybrid ELL/COO matrix view
 *
 * \tparam Matrix1 Type of \c ell
 * \tparam Matrix2 Type of \c coo
 * \tparam IndexType Type used for matrix indices (e.g. \c int).
 * \tparam ValueType Type used for matrix values (e.g. \c float).
 * \tparam MemorySpace A memory space (e.g. \c cusp::host_memory or cusp::device_memory)
 *
 */
template <typename Matrix1,
          typename Matrix2,
          typename IndexType   = typename Matrix1::index_type,
          typename MemorySpace = typename cusp::minimum_space<typename Matrix1::memory_space, typename Matrix2::memory_space>::type >
class hybb_matrix_view : public detail::binary_matrix_base<IndexType,MemorySpace,cusp::hybb_format>
{
  typedef cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::hybb_format> Parent;
  public:
    /*! type of \p ELL portion of the HYB structure
     */
    typedef Matrix1 ellb_matrix_type;
    
    /*! type of \p COO portion of the HYB structure 
     */
    typedef Matrix2 coob_matrix_type;

    /*! equivalent container type
     */
    typedef typename cusp::hybb_matrix<IndexType, MemorySpace> container;

    /*! equivalent view type
     */
    typedef typename cusp::hybb_matrix_view<Matrix1, Matrix2, IndexType, MemorySpace> view;

    /*! View to the \p ELL portion of the HYB structure.
     */
    ellb_matrix_type ell;
    
    /*! View to the \p COO portion of the HYB structure.
     */
    coob_matrix_type coo;

    /*! Construct an empty \p hybb_matrix_view.
     */
    hybb_matrix_view() {}

    template <typename OtherMatrix1, typename OtherMatrix2>
    hybb_matrix_view(OtherMatrix1& ell, OtherMatrix2& coo)
    : Parent(ell.num_rows, ell.num_cols, ell.num_entries + coo.num_entries), ell(ell), coo(coo) {}
    
    template <typename OtherMatrix1, typename OtherMatrix2>
    hybb_matrix_view(const OtherMatrix1& ell, const OtherMatrix2& coo)
    : Parent(ell.num_rows, ell.num_cols, ell.num_entries + coo.num_entries), ell(ell), coo(coo) {}

    template <typename Matrix>
    hybb_matrix_view(Matrix& A)
    : Parent(A), ell(A.ell), coo(A.coo) {}
    
    template <typename Matrix>
    hybb_matrix_view(const Matrix& A)
    : Parent(A), ell(A.ell), coo(A.coo) {}

    /*! Resize matrix dimensions and underlying storage
     */
    void resize(size_t num_rows, size_t num_cols,
                size_t num_ell_entries, size_t num_coo_entries,
                size_t num_entries_per_row, size_t alignment = 32)
    {
      Parent::resize(num_rows, num_cols, num_ell_entries + num_coo_entries);
      ell.resize(num_rows, num_cols, num_ell_entries, num_entries_per_row, alignment);
      coo.resize(num_rows, num_cols, num_coo_entries);
    }
};
/*! \} // end Views
 */

} // end namespace cusp

#include <cusp/detail/hybb_matrix.inl>