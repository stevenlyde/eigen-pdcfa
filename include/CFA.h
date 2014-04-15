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
#include <cusparse_v2.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include <helper_cuda.h>

//openmp
#include <omp.h>

//sparse matrix
#include "sparse.h"
#include "matrix_types.h"

//cusp
#include <cusp/dia_matrix.h>
#include <cusp/ell_matrix.h>
#include <cusp/csr_matrix.h>
//#include <cusp/hybb_matrix.h>
#include <cusp/coo_matrix.h>
//#include <cusp/coob_matrix.h>
#include <cusp/io/matrix_market.h>
#include <cusp/multiply.h>
#include <cusp/elementwise.h>
#include <cusp/transpose.h>
#include <cusp/blas.h>
#include <cusp/print.h>

#define NUM_THREADS     8
#define ARG_MAX         512
#define BLOCKS          32
#define BLOCK_THREADS   384

#define CPU         0
#define GPU         1
#define BUILD_TYPE  GPU         //0 is CPU 1 is GPU

bool compare_entry(const std::pair<int,int> &a, const std::pair<int,int> &b)
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

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
class CFA
{
private:
    int m_numThreads;
    int m_maxCall;
    int m_maxList;

    bool debug;

#if(BUILD_TYPE == CPU)
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> sigma;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> sigma_prime;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> CondTrue;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> CondFalse;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> Body;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> Fun;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> Arg[ARG_MAX];
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> Var[ARG_MAX];
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> temp_Mat[8];
#elif(BUILD_TYPE == GPU)
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> sigma;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> sigma_prime;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> CondTrue;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> CondFalse;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> Body;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> Fun;
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> Arg[ARG_MAX];
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> Var[ARG_MAX];
    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> temp_Mat[8];
#endif

#if(BUILD_TYPE == GPU)
    cudaStream_t stream_Call;
    cudaStream_t stream_List;
    cudaStream_t stream_Set;
    cudaStream_t stream_IF;
    cudaStream_t stream_Num;
    cudaStream_t stream_Bool;
    cudaStream_t stream_Void;
#endif

    cusp::array1d<VALUE_TYPE, MEM_TYPE> Call[ARG_MAX];
    cusp::array1d<VALUE_TYPE, MEM_TYPE> r;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> r_prime;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> IF;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> SET;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> PrimBool;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> PrimVoid;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> PrimNum;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> PrimList[ARG_MAX];
    cusp::array1d<VALUE_TYPE, MEM_TYPE> a[ARG_MAX];
    cusp::array1d<VALUE_TYPE, MEM_TYPE> a_var;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> a_set;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> v[ARG_MAX];
    cusp::array1d<VALUE_TYPE, MEM_TYPE> vf;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> v_set;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> v_cond;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> v_list;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> s;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> s_indices;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> temp_indices;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> AND_vec1;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> AND_vec2;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> Fun_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> Body_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> Arg_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> Cond_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> accum_var_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> accum_vf_vec;

    void f_call();
    void f_list();
    void f_set();
    void f_if();
    void f_primBool();
    void f_primNum();
    void f_primVoid();

#if(BUILD_TYPE == CPU)
    void f_call_host(const cusp::array1d<VALUE_TYPE, cusp::host_memory> &s, const int j);
    void f_list_host(const cusp::array1d<VALUE_TYPE, cusp::host_memory> &s, const int j);
    void f_set_host(const cusp::array1d<VALUE_TYPE, cusp::host_memory> &s);
    void f_if_host(const cusp::array1d<VALUE_TYPE, cusp::host_memory> &s);
    void f_primBool_host(const cusp::array1d<VALUE_TYPE, cusp::host_memory> &s);
    void f_primNum_host(const cusp::array1d<VALUE_TYPE, cusp::host_memory> &s);
    void f_primVoid_host(const cusp::array1d<VALUE_TYPE, cusp::host_memory> &s);
#elif(BUILD_TYPE == GPU)
    void f_call_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j);
    void f_list_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s, const int j);
    void f_set_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s);
    void f_if_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s);
    void f_primBool_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s);
    void f_primNum_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s);
    void f_primVoid_device(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s);
#endif

    cusp::array1d<VALUE_TYPE, MEM_TYPE> temp_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> temp_r;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> VOID_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> NOT_FALSE_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> FALSE_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> NUM_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> BOOL_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> LIST_vec;
    cusp::array1d<VALUE_TYPE, MEM_TYPE> tb, fb;

    dim3 GRID, BLOCK;

public:
    CFA() : m_maxCall(0), m_maxList(0) {};
    CFA(char *filename) : m_maxCall(0), m_maxList(0)
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
    int gpuDeviceInit(int devID);

    //CPU calls
    void CPU_Init();

    //Debug functions
};

#if(BUILD_TYPE == CPU)
template <typename VALUE_TYPE>
void AND_OP(    const cusp::array1d<VALUE_TYPE, cusp::host_memory> &A, 
                const cusp::array1d<VALUE_TYPE, cusp::host_memory> &B,
                std::vector<VALUE_TYPE> &vec);

template <typename VALUE_TYPE>
void AccumVec(  cusp::array1d<VALUE_TYPE, cusp::host_memory> &a,
                const cusp::array1d<VALUE_TYPE, cusp::host_memory> &b);

template <typename VALUE_TYPE>
cusp::csr_matrix<int, VALUE_TYPE, cusp::host_memory>& 
OuterProduct(   const cusp::array1d<VALUE_TYPE, cusp::host_memory> &a,
                const cusp::array1d<VALUE_TYPE, cusp::host_memory> &b,
                cusp::csr_matrix<int, VALUE_TYPE, cusp::host_memory> &mat);
#endif      //CPU

#if(BUILD_TYPE == GPU)

template <typename VALUE_TYPE>
void AND_OP(const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
            const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
            cusp::array1d<VALUE_TYPE, cusp::device_memory> &c);

template <typename VALUE_TYPE>
void get_indices(   const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
                    cusp::array1d<VALUE_TYPE, cusp::device_memory> &b);

template <typename VALUE_TYPE>
void AccumVec(  cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
                const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b);

template <typename INDEX_TYPE, typename VALUE_TYPE>
void AccumMat(  const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &mat,
                cusp::array1d<VALUE_TYPE, cusp::device_memory> &vec);

template <typename INDEX_TYPE, typename VALUE_TYPE>
void column_select( const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &A,
                    const cusp::array1d<VALUE_TYPE, cusp::device_memory> &s,
                    const INDEX_TYPE index,
                    cusp::array1d<VALUE_TYPE, cusp::device_memory> &y);

template <typename INDEX_TYPE, typename VALUE_TYPE>
void OuterProduct(  const cusp::array1d<VALUE_TYPE, cusp::device_memory> &a,
                    const cusp::array1d<VALUE_TYPE, cusp::device_memory> &b,
                    cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat);

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_add(   cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &A,
                cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &B,
                cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &C);

template <typename INDEX_TYPE, typename VALUE_TYPE>
void ell_spmv(  const cusp::ell_matrix<int, VALUE_TYPE, cusp::device_memory> &A,
                const cusp::array1d<VALUE_TYPE, cusp::device_memory> &x,
                cusp::array1d<VALUE_TYPE, cusp::device_memory> &y);

#endif      //GPU

#endif