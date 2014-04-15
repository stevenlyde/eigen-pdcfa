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

/*! \file coob_matrix.h
 *  \brief Coordinate matrix format
 */

#pragma once

#include <cusp/detail/config.h>

#include <cusp/format.h>
#include <cusp/array1d.h>
#include <cusp/detail/matrix_base.h>

namespace cusp
{

// forward definition
template <typename Array1, typename Array2, typename IndexType, typename MemorySpace> class coob_matrix_view;

/*! \p coob_matrix : Coordinate matrix container
 */
template <typename IndexType, class MemorySpace>
class coob_matrix : public detail::binary_matrix_base<IndexType,MemorySpace,cusp::coob_format>
{
  typedef cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::coob_format> Parent;
  public:
    /*! rebind matrix to a different MemorySpace
     */
    template<typename MemorySpace2>
    struct rebind { typedef cusp::coob_matrix<IndexType, MemorySpace2> type; };
        
    /*! type of \c row_indices array
     */
    typedef typename cusp::array1d<IndexType, MemorySpace> row_indices_array_type;
    
    /*! type of \c column_indices array
     */
    typedef typename cusp::array1d<IndexType, MemorySpace> column_indices_array_type;
    
    /*! equivalent container type
     */
    typedef typename cusp::coob_matrix<IndexType, MemorySpace> container;

    /*! equivalent view type
     */
    typedef typename cusp::coob_matrix_view<typename row_indices_array_type::view,
                                           typename column_indices_array_type::view,
                                           IndexType, MemorySpace> view;
    
    /*! equivalent const_view type
     */
    typedef typename cusp::coob_matrix_view<typename row_indices_array_type::const_view,
                                           typename column_indices_array_type::const_view,
                                           IndexType, MemorySpace> const_view;

    /*! Storage for the row indices of the COO data structure.
     */
    row_indices_array_type row_indices;
    
    /*! Storage for the column indices of the COO data structure.
     */
    column_indices_array_type column_indices;

    /*! Construct an empty \p coob_matrix.
     */
    coob_matrix() {}

    /*! Construct a \p coob_matrix with a specific shape and number of nonzero entries.
     *
     *  \param num_rows Number of rows.
     *  \param num_cols Number of columns.
     *  \param num_entries Number of nonzero matrix entries.
     */
    coob_matrix(size_t num_rows, size_t num_cols, size_t num_entries)
      : Parent(num_rows, num_cols, num_entries),
        row_indices(num_entries), column_indices(num_entries) {}

    /*! Construct a \p coob_matrix from another matrix.
     *
     *  \param matrix Another sparse or dense matrix.
     */
    template <typename MatrixType>
    coob_matrix(const MatrixType& matrix);

    /*! Resize matrix dimensions and underlying storage
     */
    void resize(size_t num_rows, size_t num_cols, size_t num_entries)
    {
      Parent::resize(num_rows, num_cols, num_entries);
      row_indices.resize(num_entries);
      column_indices.resize(num_entries);
    }

    /*! Swap the contents of two \p coob_matrix objects.
     *
     *  \param matrix Another \p coob_matrix with the same IndexType and ValueType.
     */
    void swap(coob_matrix& matrix)
    {
      Parent::swap(matrix);
      row_indices.swap(matrix.row_indices);
      column_indices.swap(matrix.column_indices);
    }

    /*! Assignment from another matrix.
     *
     *  \param matrix Another sparse or dense matrix.
     */
    template <typename MatrixType>
    coob_matrix& operator=(const MatrixType& matrix);

    /*! Sort matrix elements by row index
     */
    void sort_by_row(void);
    
    /*! Sort matrix elements by row and column index
     */
    void sort_by_row_and_column(void);
    
    /*! Determine whether matrix elements are sorted by row index
     *
     *  \return \c false, if the row indices are unsorted; \c true, otherwise.
     */
    bool is_sorted_by_row(void);
    
    /*! Determine whether matrix elements are sorted by row and column index
     *
     *  \return \c false, if the row and column indices are unsorted; \c true, otherwise.
     */
    bool is_sorted_by_row_and_column(void);
}; // class coob_matrix
/*! \}
 */

/*! \addtogroup sparse_matrix_views Sparse Matrix Views
 *  \ingroup sparse_matrices
 *  \{
 */

/*! \p coob_matrix_view : Coordinate matrix view
 *
 * \tparam Array1 Type of \c row_indices array view
 * \tparam Array2 Type of \c column_indices array view
 * \tparam IndexType Type used for matrix indices (e.g. \c int).
 * \tparam MemorySpace A memory space (e.g. \c cusp::host_memory or cusp::device_memory)
 *
 */
template <typename Array1,
          typename Array2,
          typename IndexType   = typename Array1::value_type,
          typename MemorySpace = typename cusp::minimum_space<typename Array1::memory_space, typename Array2::memory_space>::type >
          class coob_matrix_view : public cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::coob_format>
{
  typedef cusp::detail::binary_matrix_base<IndexType,MemorySpace,cusp::coob_format> Parent;
  public:
    typedef Array1 row_indices_array_type;
    typedef Array2 column_indices_array_type;
    
    /*! equivalent container type
     */
    typedef typename cusp::coob_matrix<IndexType, MemorySpace> container;

    /*! equivalent view type
     */
    typedef typename cusp::coob_matrix_view<Array1, Array2, IndexType, MemorySpace> view;

    /*! View of the row indices of the COO data structure.  Also called the "row pointer" array.
     */
    row_indices_array_type row_indices;
    
    /*! View of the column indices of the COO data structure.
     */
    column_indices_array_type column_indices;

    // construct empty view
    coob_matrix_view(void)
      : Parent() {}

    // construct from existing COO matrix or view
    template <typename Matrix>
    coob_matrix_view(Matrix& A)
      : Parent(A),
        row_indices(A.row_indices),
        column_indices(A.column_indices) {}
  
    // TODO check sizes here
    coob_matrix_view(size_t num_rows,
                    size_t num_cols,
                    size_t num_entries,
                    Array1 row_indices,
                    Array2 column_indices)
      : Parent(num_rows, num_cols, num_entries),
        row_indices(row_indices),
        column_indices(column_indices) {}

    /*! Resize matrix dimensions and underlying storage
     */
    void resize(size_t num_rows, size_t num_cols, size_t num_entries)
    {
      Parent::resize(num_rows, num_cols, num_entries);
      row_indices.resize(num_entries);
      column_indices.resize(num_entries);
    }
    
    /*! Sort matrix elements by row index
     */
    void sort_by_row(void);
    
    /*! Sort matrix elements by row and column index
     */
    void sort_by_row_and_column(void);
    
    /*! Determine whether matrix elements are sorted by row index
     *
     *  \return \c false, if the row indices are unsorted; \c true, otherwise.
     */
    bool is_sorted_by_row(void);
    
    /*! Determine whether matrix elements are sorted by row and column index
     *
     *  \return \c false, if the row and column indices are unsorted; \c true, otherwise.
     */
    bool is_sorted_by_row_and_column(void);
};

/* Convenience functions */

template <typename Array1,
          typename Array2>
coob_matrix_view<Array1,Array2>
make_coob_matrix_view(size_t num_rows,
                     size_t num_cols,
                     size_t num_entries,
                     Array1 row_indices,
                     Array2 column_indices)
{
  return coob_matrix_view<Array1,Array2>
    (num_rows, num_cols, num_entries,
     row_indices, column_indices);
}

template <typename Array1,
          typename Array2,
          typename IndexType,
          typename MemorySpace>
coob_matrix_view<Array1,Array2,IndexType,MemorySpace>
make_coob_matrix_view(const coob_matrix_view<Array1,Array2,IndexType,MemorySpace>& m)
{
  return coob_matrix_view<Array1,Array2,IndexType,MemorySpace>(m);
}
    
template <typename IndexType, class MemorySpace>
typename coob_matrix<IndexType,MemorySpace>::view
make_coob_matrix_view(coob_matrix<IndexType,MemorySpace>& m)
{
  return make_coob_matrix_view
    (m.num_rows, m.num_cols, m.num_entries,
     make_array1d_view(m.row_indices),
     make_array1d_view(m.column_indices));
}

template <typename IndexType, class MemorySpace>
typename coob_matrix<IndexType,MemorySpace>::const_view
make_coob_matrix_view(const coob_matrix<IndexType,MemorySpace>& m)
{
  return make_coob_matrix_view
    (m.num_rows, m.num_cols, m.num_entries,
     make_array1d_view(m.row_indices),
     make_array1d_view(m.column_indices));
}
/*! \}
 */


} // end namespace cusp

#include <cusp/detail/coob_matrix.inl>