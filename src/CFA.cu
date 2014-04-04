#include "CFA.cuh"
#include "CFA.h"

#define BLOCK_SIZE	256
#define DEBUG	1

#if(DEBUG == 1)
#define MULTIPLY(A, B, C)		cusp::multiply(A, B, C);\ 
								fprintf(stderr, "(%d,%d) x (%d,%d) -> (%d,%d)\n", A.num_rows, A.num_cols, B.num_rows, B.num_cols, C.num_rows, C.num_cols)

#define DEBUG_PRINT(A, B)		fprintf(stderr, A);\
								cusp::print(B);
#else
#define MULTIPLY(A, B, C)		cusp::multiply(A, B, C)
#define DEBUG_PRINT(A, B)
#endif

#include "host_forms.inl"
#include "device_forms.inl"

// template <typename INDEX_TYPE, typename MEM_TYPE>
// inline void subtract_vec(cusp::array1d<INDEX_TYPE, MEM_TYPE> &A,
// 							cusp::array1d<INDEX_TYPE, MEM_TYPE> &B,
// 							cusp::array1d<INDEX_TYPE, MEM_TYPE> &C)
// {
// 	thrust::transform(A.begin(), A.end(), B.begin(), C.begin(), thrust::minus<INDEX_TYPE>());
// }

// General GPU Device CUDA Initialization
template <typename INDEX_TYPE, typename MEM_TYPE>
int CFA<INDEX_TYPE, MEM_TYPE>::gpuDeviceInit(int devID)
{
    int deviceCount;
    checkCudaErrors(cudaGetDeviceCount(&deviceCount));

    if (deviceCount == 0)
    {
        fprintf(stderr, "gpuDeviceInit() CUDA error: no devices supporting CUDA.\n");
        exit(-1);
    }

    if (devID < 0)
       devID = 0;
        
    if (devID > deviceCount-1)
    {
        fprintf(stderr, "\n");
        fprintf(stderr, ">> %d CUDA capable GPU device(s) detected. <<\n", deviceCount);
        fprintf(stderr, ">> gpuDeviceInit (-device=%d) is not a valid GPU device. <<\n", devID);
        fprintf(stderr, "\n");
        return -devID;
    }

    cudaDeviceProp deviceProp;
    checkCudaErrors( cudaGetDeviceProperties(&deviceProp, devID) );

    if (deviceProp.major < 1)
    {
        fprintf(stderr, "gpuDeviceInit(): GPU device does not support CUDA.\n");
        exit(-1);
    }
    
    checkCudaErrors( cudaSetDevice(devID) );
    printf("gpuDeviceInit() CUDA Device [%d]: \"%s\n", devID, deviceProp.name);

    return devID;
}

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::AND_OP(	const cusp::array1d<INDEX_TYPE, MEM_TYPE> &A, 
										const cusp::array1d<INDEX_TYPE, MEM_TYPE> &B, 
										cusp::array1d<INDEX_TYPE, MEM_TYPE> &C)
{
	assert(A.size() == B.size());
	C.resize(A.size(), 0);

	for(int i=0; i<A.size(); ++i)
		C[i] = A[i] & B[i];
}

// template <typename IndexType, typename MemorySpace>
// __launch_bounds__(BLOCK_SIZE,1)
// __global__ void CFA<INDEX_TYPE, MEM_TYPE>::AND_OP(	const IndexType *A, 
// 										const IndexType *B, 
// 										IndexType *C,
// 										const IndexType size)
// {
// 	const int threadID = blockIdx.x*blockDim.x + threadIdx.x;
// 	const int grid_size = blockDim.x*gridDim.x;

// 	for(int i=threadID; i<size; i+=grid_size)
// 		C[i] = A[i] & B[i];
// }

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::Init()
{
	tb.resize(1, 0);
	fb.resize(1, 0);
	VOID_vec.resize(sigma.num_cols, 0);
	NOT_FALSE_vec.resize(sigma.num_cols, 1);
	FALSE_vec.resize(sigma.num_cols, 0);
	BOOL_vec.resize(sigma.num_cols, 0);
	NUM_vec.resize(sigma.num_cols, 0);

	int max = sigma.num_cols;
	for(int i=0; i<max; ++i)
	{
		if(i == max - 4)			//void
			VOID_vec[i] = 1;
		else if(i == max - 3)		//#t
			BOOL_vec[i] = 1;
		else if(i == max - 2)		//#f
		{
			NOT_FALSE_vec[i] = 0;
			BOOL_vec[i] = 1;
			FALSE_vec[i] = 1;
		}
		else if(i == max - 1)		//NUM
			NUM_vec[i] = 1;
	}

	m_ValuesSize = sigma.num_rows;
}

// template <typename INDEX_TYPE, typename MEM_TYPE>
// void CFA<INDEX_TYPE, MEM_TYPE>::Vectorize(	cusp::ell_matrix<int,char,cusp::host_memory> &A, 
// 												cusp::ell_matrix<int,char,cusp::host_memory> &B)
// {
// 	B.resize(1, A.num_rows*A.num_cols, A.num_entries);
// 	for(int i=0; i<A.column_indices.size(); ++i)
// 	{
// 		B.row_indices[i] = 0;
// 		B.column_indices[i] = A.row_indices[i]*A.num_cols + A.column_indices[i];
// 		B.values[i] = 1;
// 	}
// 	B.sort_by_row_and_column();
// }

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::ReadTestFile(const char* filename)
{
	std::ifstream tf(filename);
	if(tf.fail())
	{
		fprintf(stderr, "Invalid test file: %s\n", filename);
		exit(1);
	}

	fprintf(stderr, "Reading test file: %s\n", filename);
	while(!tf.eof())
	{
		int rows, cols, i, j;
		char buf[64], name[32];

		tf.getline(buf, 64);
		if(tf.gcount() > 1)
		{
			std::sscanf(buf, "%s %d %d", name, &rows, &cols);
			//std::cout << name << "  " << rows << "  " << cols << std::endl;

			std::vector< std::pair<int, int> > indices;
			tf.getline(buf, 64);
			while(tf.gcount() > 1)
			{
				std::sscanf(buf, "%d %d", &i, &j);
				indices.push_back(std::make_pair(i,j));
				//fprintf(stderr, "gcout: %d %d %d\n", tf.gcount(), x, y);
				tf.getline(buf, 64);
			}
			std::sort(indices.begin(), indices.end());

			#define NUM_ENTRIES_PER_ROW		32
			cusp::coob_matrix<int,cusp::host_memory> A(rows, cols, indices.size()), B;
			for(int i=0; i<indices.size(); ++i)
			{
				A.row_indices[i] = indices[i].first;
				A.column_indices[i] = indices[i].second;
			}
			A.sort_by_row_and_column();

			cusp::array1d<INDEX_TYPE, MEM_TYPE> vec;
			if(A.num_cols == 1)
			{
				vec.resize(A.num_rows, 0);
				for(int i=0; i<A.num_entries; ++i)
					vec[A.row_indices[i]] = 1;
			}

			fprintf(stderr, "\n%s (%d x %d)\n", name, A.num_rows, A.num_cols);
			cusp::transpose(A, B);
			//cusp::print(A);

			std::string mat_name(name), sname = "", snum = "";
			int mat_num = -1;
			for(int i=0; i<NUM_MATRIX_TYPES; ++i)
			{
				std::string find_name = MatrixTypeMap[i];
				if(mat_name.find(find_name) == 0)
				{
					sname = find_name;
					snum = mat_name.substr(sname.size());
					if(snum.size() > 0)
						mat_num = atoi(snum.c_str()) - 1;
					break;
				}
			}

			//fprintf(stderr, "mat_name: %s \t\t sname: %s\n", mat_name.c_str(), sname.c_str());
			//parse name
			if(sname == "r")
				r = vec;
			else if(sname == "sigma")
				sigma = A;
			else if(sname == "CondTrue")
				CondTrue = A;
			else if(sname == "CondFalse")
				CondFalse = A;
			else if(sname == "Body")
				Body = A;
			else if(sname == "Fun")
				Fun = A;
			else if(sname == "Arg")
				Arg[mat_num] = A;
			else if(sname == "Var")
				Var[mat_num] = A;
			else if(sname == "Call")
			{
				Call[mat_num] = B;
				if(m_maxArgs < mat_num + 1)
					m_maxArgs = mat_num + 1;
			}
			else if(sname == "PrimBool")
				PrimBool = vec;
			else if(sname == "PrimNum")
				PrimNum = vec;
			else if(sname == "PrimVoid")
				PrimVoid = vec;
			else if(sname == "If")
				IF = vec;
			else if(sname == "Set")
				SET = vec;
			else
				fprintf(stderr, "could not match input matrix: %s\n", name);
		}
	}

	tf.close();
}

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::Run_Analysis()
{
	fprintf(stderr, "\n\n\nStarting analysis...\n");

	//kernel calls
	cusp::array1d<INDEX_TYPE, MEM_TYPE> s(sigma.num_cols, 0);
	fprintf(stderr, "m_maxArgs: %d\n", m_maxArgs);
	bool sigma_change = false, r_change = false;
	r_prime = r;
	sigma_prime = sigma;
	int iter=0;

	do
	{
		//CPU execution
		fprintf(stderr, "\n\nITERATION %d\n\n", ++iter);
		//f_call
		// fprintf(stderr, "f_call\n");
	 // 		for(int j=1; j<=m_maxArgs; ++j)
		// {
		// 	//r & Call
		// 	AND_OP(r, Call[j-1], s);
		// 	f_call(s, j);
		// }

		//f_set
		fprintf(stderr, "f_set\n");
		AND_OP(r, SET, s);
		f_set(s);

		//f_if
		// fprintf(stderr, "f_if\n");
		// AND_OP(r, IF, s);
		// f_if(s);

		// //f_primBool
		// fprintf(stderr, "f_primBool\n");
		// AND_OP(r, PrimBool, s);
		// f_primBool(s);

		// //f_primNum
		// fprintf(stderr, "f_primNum\n");
		// AND_OP(r, PrimInt, s);
		// f_primNum(s);

		// //f_primVoid
		// fprintf(stderr, "f_PrimVoid\n");
		// AND_OP(r, PrimVoid, s);
		// f_primVoid(s);

		//correct values...  (this won't be a problem in later versions)
		// for(int i=0; i<sigma_prime.num_entries; ++i)
		// 	sigma_prime.values[i] = 1;
		// for(int i=0; i<r_prime.num_entries; ++i)
		// 	r_prime.values[i] = 1;

		// fprintf(stderr, "\n\nupdate sigma\n");
		// cusp::subtract(sigma, sigma_prime, temp_Mat[0]);
		// fprintf(stderr, "***sigma difference***\n");
		// cusp::print(temp_Mat[0]);
		// sigma_change = false;
		// for(int i=0; i<temp_Mat[0].num_entries; ++i)
		// 	if(temp_Mat[0].values[i] != 0)
		// 	{
		// 		sigma_change = true;
		// 		break;
		// 	}

		fprintf(stderr, "\n\nupdate r\n");
		//subtract_vec(r, r_prime, temp_vec);
		thrust::transform(r.begin(), r.end(), r_prime.begin(), temp_vec.begin(), thrust::minus<INDEX_TYPE>());
		// fprintf(stderr, "***r difference***\n");
		// cusp::print(temp_vec);
		r_change = false;
		for(int i=0; i<temp_vec.size(); ++i)
			if(temp_vec[i] != 0)
			{
				r_change = true;
				break;
			}

		sigma_prime.sort_by_row_and_column();
		r = r_prime;
		sigma = sigma_prime;

//#elif(MEM_TYPE == DEVICE)
		//GPU kernel invocation

		//dim3 GRID, BLOCK;

		//GPU_Iterate<<<GRID, BLOCK>>> ();
		//read back change vectors

//#endif
	} while(r_change || sigma_change);

	fprintf(stderr, "Analysis Complete...\n");
}

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::WriteStore()
{
	fprintf(stderr, "\nsigma:\n");
	cusp::print(sigma);
	fprintf(stderr, "\nr:\n");
	cusp::print(r);
	std::ofstream output("tests/output.dat");

	output << "sigma " << sigma.num_cols << " " << sigma.num_rows << std::endl;
	for(int i=0; i<sigma.num_entries; ++i)
	{
		output << sigma.row_indices[i] << " " << sigma.column_indices[i] << " " << std::endl;
	}

	output.close();
}

void CPU_Test(std::string filename)
{
	CFA<char, cusp::host_memory> Analysis;
	//Analysis.gpuDeviceInit(1);

	Analysis.ReadTestFile(filename.c_str());
	Analysis.Init();
	//Analysis.PrintMatrices();

	// double startTime = omp_get_wtime();
	// Analysis.Run_Analysis();
	// double endTime = omp_get_wtime();

	// fprintf(stderr, "Run Time: %f seconds\n", endTime - startTime);
	// Analysis.WriteStore();
}

void GPU_Test()
{

}