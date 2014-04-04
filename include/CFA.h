#ifndef CFA_H
#define CFA_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

// includes CUDA
#include <cuda.h>
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <helper_cuda.h>

//openmp
#include <omp.h>

//sparse matrix
#include "sparse.h"
#include "matrix_types.h"

//cusp
//#include <cusp/ell_matrix.h>
//#include <cusp/csr_matrix.h>
#include <cusp/hybb_matrix.h>
#include <cusp/coob_matrix.h>
#include <cusp/coob_matrix.h>
#include <cusp/coob_matrix.h>
#include <cusp/io/matrix_market.h>
#include <cusp/multiply.h>
#include <cusp/elementwise.h>
#include <cusp/transpose.h>
#include <cusp/blas.h>
#include <cusp/print.h>

#define NUM_THREADS     8
#define ARG_MAX         32

inline bool compare_entry(std::pair<int,int> &a, std::pair<int,int> &b)
{
    //first -> row
    //second -> column
    if(a.first < b.first)
        return true;
    else if(b.first < a.first)
        return false;
    else
        return (a.second < b.second);
}

// General GPU Device CUDA Initialization
int gpuDeviceInit(int devID);

template <typename INDEX_TYPE, typename MEM_TYPE>
class CFA
{
private:
    int m_numThreads;
    int m_maxArgs;
    int m_ValuesSize;

    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> sigma;
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> sigma_prime;
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> CondTrue;
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> CondFalse;
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> Body;
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> Fun;
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> temp_Mat[8];

    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> Arg[ARG_MAX];
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> Var[ARG_MAX];
    cusp::coob_matrix<INDEX_TYPE,MEM_TYPE> Call[ARG_MAX];
    
    cusp::array1d<INDEX_TYPE, MEM_TYPE> r;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> r_prime;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> a[ARG_MAX];
    cusp::array1d<INDEX_TYPE, MEM_TYPE> a_var;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> a_set;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> v[ARG_MAX];
    cusp::array1d<INDEX_TYPE, MEM_TYPE> vf;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> v_set;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> v_cond;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> IF;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> SET;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> PrimBool;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> PrimVoid;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> PrimNum;

    //void f_call(const cusp::array1d<INDEX_TYPE, MEM_TYPE> &s, const int j);
    void f_set(const cusp::array1d<INDEX_TYPE, MEM_TYPE> &s);
    //void f_if(const cusp::array1d<INDEX_TYPE, MEM_TYPE> &s);
    // void f_primBool(const INDEX_TYPE s);
    // void f_primNum(const INDEX_TYPE s);
    // void f_primVoid(const INDEX_TYPE s);
    void AND_OP(    const cusp::array1d<INDEX_TYPE, MEM_TYPE> &A, 
                    const cusp::array1d<INDEX_TYPE, MEM_TYPE> &B,
                    cusp::array1d<INDEX_TYPE, MEM_TYPE> &C);

    cusp::array1d<INDEX_TYPE, MEM_TYPE> temp_vec;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> VOID_vec;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> NOT_FALSE_vec;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> FALSE_vec;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> NUM_vec;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> BOOL_vec;
    cusp::array1d<INDEX_TYPE, MEM_TYPE> tb, fb;

public:
    CFA() : m_maxArgs(0) {};
    CFA(char *filename) : m_maxArgs(0)
    {   
        ReadTestFile(filename);
        m_numThreads = NUM_THREADS;
        Init();
    };
    ~CFA() {};

    //setup and runtime functions
    void ReadTestFile(const char* filename);
    void Init();
    void Run_Analysis();
    void WriteStore();

    //GPU calls
    void GPU_Init();
    void GPU_UnionStore();
    void GPU_Iterate();
    int gpuDeviceInit(int devID);

    //CPU calls
    void CPU_Init();
    void CPU_UnionStore();

    // void Vectorize( cusp::ell_matrix<int,INDEX_TYPE,cusp::host_memory> &A, 
    //                 cusp::ell_matrix<int,INDEX_TYPE,cusp::host_memory> &B);
    // void UnionStore();

    //Debug functions
};

#endif