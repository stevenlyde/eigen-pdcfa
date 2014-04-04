#include <cstdlib>
#include <cstdio>

// includes CUDA
#include <cuda_runtime.h>

//openmp
#include <omp.h>

// General GPU Device CUDA Initialization
int gpuDeviceInit(int devID);

class CFA
{
private:


public:
    CFA() {};
    CFA(char *filename) 
    {   
        ReadTestFile(filename); 
    };
    ~CFA() {};

    //GPU calls
    void GPU_Init();
    void GPU_CFA(const int k);

    //CPU calls
    void CPU_Init();
    void CPU_CFA(const int k);

    void ReadTestFile(char* filename);
}