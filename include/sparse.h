#ifndef SPARSE_H
#define SPARSE_H

#include <cusp/array1d.h>
#include <cusp/print.h>
#include <thrust/sort.h>
#include <thrust/reduce.h>
#include <thrust/scan.h>

//#include "matrix_base.h"

struct host {};
struct device {};

//cusp and thrust
typedef cusp::array1d<char, cusp::device_memory> 	CuspVectorChar_d;
typedef cusp::array1d<char, cusp::host_memory> 		CuspVectorChar_h;
typedef cusp::array1d<short, cusp::device_memory> 	CuspVectorShort_d;
typedef cusp::array1d<short, cusp::host_memory> 	CuspVectorShort_h;
typedef cusp::array1d<int, cusp::device_memory> 	CuspVectorInt_d;
typedef cusp::array1d<int, cusp::host_memory> 		CuspVectorInt_h;
typedef cusp::array1d<double, cusp::device_memory> 	CuspVector_d;
typedef cusp::array1d<double, cusp::host_memory> 	CuspVector_h;
#define TPC(x)		thrust::raw_pointer_cast(x)

#endif