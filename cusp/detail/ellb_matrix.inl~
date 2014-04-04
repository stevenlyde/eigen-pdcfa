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

#include <cusp/convert.h>
#include <cusp/detail/utils.h>

namespace cusp
{

//////////////////
// Constructors //
//////////////////
        
// construct from a different matrix
template <typename IndexType, class MemorySpace>
template <typename MatrixType>
ellb_matrix<IndexType,MemorySpace>
    ::ellb_matrix(const MatrixType& matrix)
    {
        cusp::convert(matrix, *this);
    }

//////////////////////
// Member Functions //
//////////////////////

// assignment from another matrix
template <typename IndexType, class MemorySpace>
template <typename MatrixType>
    ellb_matrix<IndexType,MemorySpace>&
    ellb_matrix<IndexType,MemorySpace>
    ::operator=(const MatrixType& matrix)
    {
        cusp::convert(matrix, *this);
        
        return *this;
    }

///////////////////////////
// Convenience Functions //
///////////////////////////

template <typename Array1>
ellb_matrix_view<Array1>
make_ellb_matrix_view(size_t num_rows,
                     size_t num_cols,
                     size_t num_entries,
                     Array1 column_indices)
{
  return ellb_matrix_view<Array1>
    (num_rows, num_cols, num_entries, column_indices);
}

template <typename Array1,
          typename IndexType,
          typename MemorySpace>
ellb_matrix_view<Array1,IndexType,MemorySpace>
make_ellb_matrix_view(const ellb_matrix_view<Array1,IndexType,MemorySpace>& m)
{
  return ellb_matrix_view<Array1,IndexType,MemorySpace>(m);
}
    
template <typename IndexType, class MemorySpace>
typename ellb_matrix<IndexType,MemorySpace>::view
make_ellb_matrix_view(ellb_matrix<IndexType,MemorySpace>& m)
{
  return make_ellb_matrix_view
    (m.num_rows, m.num_cols, m.num_entries,
     cusp::make_array2d_view(m.column_indices));
}

template <typename IndexType, class MemorySpace>
typename ellb_matrix<IndexType,MemorySpace>::const_view
make_ellb_matrix_view(const ellb_matrix<IndexType,MemorySpace>& m)
{
  return make_ellb_matrix_view
    (m.num_rows, m.num_cols, m.num_entries,
     cusp::make_array2d_view(m.column_indices));
}

} // end namespace cusp