#include "CFA.cuh"
#include "CFA.h"

#define DEBUG		1

#if(DEBUG == 1)
#define MV_MULTIPLY(A, B, C)	cusp::multiply(A, B, C);\
								fprintf(stdout, "(%dx%d) * (%dx1) -> (%dx1)\n", A.num_rows, A.num_cols, B.size(), C.size())

#define DEBUG_PRINT(A, B)		fprintf(stdout, A);\
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
        fprintf(stdout, "gpuDeviceInit() CUDA error: no devices supporting CUDA.\n");
        exit(-1);
    }

    if (devID < 0)
       devID = 0;
        
    if (devID > deviceCount-1)
    {
        fprintf(stdout, "\n");
        fprintf(stdout, ">> %d CUDA capable GPU device(s) detected. <<\n", deviceCount);
        fprintf(stdout, ">> gpuDeviceInit (-device=%d) is not a valid GPU device. <<\n", devID);
        fprintf(stdout, "\n");
        return -devID;
    }

    cudaDeviceProp deviceProp;
    checkCudaErrors( cudaGetDeviceProperties(&deviceProp, devID) );

    if (deviceProp.major < 1)
    {
        fprintf(stdout, "gpuDeviceInit(): GPU device does not support CUDA.\n");
        exit(-1);
    }
    
    checkCudaErrors( cudaSetDevice(devID) );
    printf("gpuDeviceInit() CUDA Device [%d]: \"%s\n", devID, deviceProp.name);

    return devID;
}

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::Init()
{
	size_t vec_size = sigma.num_rows;
	VOID_vec.resize(vec_size, 0);
	NOT_FALSE_vec.resize(vec_size, 1);
	FALSE_vec.resize(vec_size, 0);
	BOOL_vec.resize(vec_size, 0);
	NUM_vec.resize(vec_size, 0); 
	LIST_vec.resize(vec_size, 0);
	tb.resize(1, 0);
	fb.resize(1, 0);
	AND_vec.resize(r.size(), 0);

	for(int i=0; i<ARG_MAX; ++i)
	{
		v[i].resize(vec_size, 0);
		a[i].resize(sigma.num_cols, 0);
		
		if(Call[i].size() != r.size())
			Call[i].resize(r.size(), 0);
		if(PrimList[i].size() != r.size())
			PrimList[i].resize(r.size(), 0);
	}
    a_var.resize(sigma.num_cols, 0);
    a_set.resize(sigma.num_cols, 0);
    vf.resize(vec_size, 0);
    v_set.resize(vec_size, 0);
    v_cond.resize(vec_size, 0);
    v_list.resize(vec_size, 0);

	for(int i=0; i<vec_size; ++i)
	{
		if(i == vec_size - 5)			//list
			LIST_vec[i] = 1;			
		else if(i == vec_size - 4)		//void
			VOID_vec[i] = 1;
		else if(i == vec_size - 3)		//#t
			BOOL_vec[i] = 1;
		else if(i == vec_size - 2)		//#f
		{
			NOT_FALSE_vec[i] = 0;
			BOOL_vec[i] = 1;
			FALSE_vec[i] = 1;
		}
		else if(i == vec_size - 1)		//NUM
			NUM_vec[i] = 1;
	}

	#if(BUILD_TYPE == GPU)
	temp_r.resize(r.size());
	temp_vec.resize(sigma.num_cols);
	temp_Body.resize(Body.num_rows, Body.num_rows, Body.num_entries, Body.num_rows/4);
	//vf_mat.resize(Body.num_rows, r.size(), Body.num_entries, Body.num_entries/4);
	temp_Fun = Fun;
	temp_Arg = Arg[0];
	print_matrix_info(temp_Mat[0]);
	temp_Mat[0].resize(sigma.num_rows, sigma.num_cols, 0, sigma.num_cols/8, 32);
	temp_Mat[1].resize(sigma.num_rows, sigma.num_cols, 0, sigma.num_cols/8, 32);
	temp_Mat[2].resize(sigma.num_rows, sigma.num_cols, 0, sigma.num_cols/8, 32);
	print_matrix_info(temp_Mat[0]);
	a_var_mat = Fun;
	for(int i=0; i<m_maxCall; ++i)
		v_mat[i].resize(Body.num_rows, r.size(), Body.num_entries, Body.num_entries/4);
	#endif

	GRID = dim3(1,1,1);
	BLOCK = dim3(BLOCK_SIZE,1,1);
}

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::ReadTestFile(const char* filename)
{
	std::ifstream tf(filename);
	if(tf.fail())
	{
		fprintf(stdout, "Invalid test file: %s\n", filename);
		exit(1);
	}

	fprintf(stdout, "Reading test file: %s\n", filename);
	while(!tf.eof())
	{
		int rows, cols, i, j;
		char buf[64], name[32];

		tf.getline(buf, 64);
		if(tf.gcount() > 1)
		{
			std::sscanf(buf, "%s %d %d", name, &rows, &cols);

			std::vector< std::pair<int, int> > indices;
			tf.getline(buf, 64);
			while(tf.gcount() > 1)
			{
				std::sscanf(buf, "%d %d", &i, &j);
				if(i<0 || i>=rows || j<0 || j>=cols)
					fprintf(stdout, "ERROR Rows: %d Cols: %d \t (i:%d j:%d)\n", rows, cols, i, j);

				indices.push_back(std::make_pair(i,j));
				tf.getline(buf, 64);
			}
			std::sort(indices.begin(), indices.end(), compare_entry);

			#define NUM_ENTRIES_PER_ROW		32
			cusp::coo_matrix<int, int, cusp::host_memory> A(rows, cols, indices.size()), B;
			for(int i=0; i<A.num_entries; ++i)
			{
				A.row_indices[i] = indices[i].first;
				A.column_indices[i] = indices[i].second;
				A.values[i] = 1;
			}
			//A.sort_by_row_and_column();

			cusp::array1d<int, cusp::host_memory> vec;
			if(A.num_cols == 1)
			{
				vec.resize(A.num_rows, 0);
				for(int i=0; i<A.num_entries; ++i)
					vec[A.row_indices[i]] = 1;
			}

			fprintf(stderr, "\n%s (%d x %d)\n", name, A.num_rows, A.num_cols);
			B.resize(A.num_rows, A.num_cols, A.num_entries);
			cusp::transpose(A, B);
			fprintf(stderr, "B: (%d x %d)\n", B.num_rows, B.num_cols);
			cusp::print(B);

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
						mat_num = atoi(snum.c_str());
					break;
				}
			}

			//parse name
			if(sname == "r")
				r = vec;
			else if(sname == "sigma")
				sigma = B;
			else if(sname == "CondTrue")
				CondTrue = B;
			else if(sname == "CondFalse")
				CondFalse = B;
			else if(sname == "Body")
				Body = B;
			else if(sname == "Fun")
				Fun = B;
			else if(sname == "Arg")
				Arg[mat_num-1] = B;
			else if(sname == "Var")
				Var[mat_num-1] = B;
			else if(sname == "Call")
			{
				Call[mat_num] = vec;
				if(m_maxCall < mat_num)
					m_maxCall = mat_num;
			}
			else if(sname == "PrimBool")
				PrimBool = vec;
			else if(sname == "PrimNum")
				PrimNum = vec;
			else if(sname == "PrimVoid")
				PrimVoid = vec;
			else if(sname == "PrimList")
			{
				PrimList[mat_num] = vec;
				if(m_maxList < mat_num)
					m_maxList = mat_num;
			}
			else if(sname == "If")
				IF = vec;
			else if(sname == "Set")
				SET = vec;
			else
				fprintf(stdout, "could not match input matrix: %s\n", name);
		}
	}

	tf.close();
}

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::Run_Analysis()
{
	fprintf(stdout, "\n\n\nStarting analysis...\n");

	s.resize(r.size(), 0);
	r_prime = r;
	sigma_prime = sigma;
	int iter=0;
	bool sigma_change = false, r_change = false;
	fprintf(stderr, "m_maxCall: %d  m_maxList: %d\n", m_maxCall, m_maxList);
	cusp::ell_matrix<int, INDEX_TYPE, cusp::host_memory> temp_sigma;
	//cusp::print(sigma);
	//typedef r.memory_space MemorySpace;

	debug = false;
	do
	{
		//CPU execution
		fprintf(stdout, "\n\nITERATION %d\n\n", ++iter);

		//f_call
		f_call();

		//f_list
		f_list();

		//f_set
		f_set();

		//f_if
		f_if();

		//f_primBool
		f_primBool();

		//f_primNum
		f_primNum();

		//f_primVoid
		f_primVoid();

		//correct values...  (this won't be a problem in later versions)
		// for(int i=0; i<sigma_prime.num_entries; ++i)
		// 	sigma_prime.values[i] = 1;
		for(int i=0; i<r_prime.size(); ++i)
			if(r_prime[i])
				r_prime[i] = 1;

		fprintf(stdout, "\nupdate sigma\n");
		// cusp::subtract(sigma_prime, sigma, temp_Mat[0]);
		if(debug)
		{
		 	fprintf(stdout, "***sigma difference***\n");
			//cusp::print(temp_Mat[0]);

			temp_sigma = sigma_prime;
			int num_entries = 0;
			int pitch = temp_sigma.column_indices.pitch;
			const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;
			for(int col=0; col<temp_sigma.column_indices.num_cols; ++col)
			{	
				int offset = pitch*col;
				for(int row=0; row<temp_sigma.num_rows; ++row, ++offset)
				{
					if(temp_sigma.column_indices.values[offset] != invalid_index)
						num_entries++;
				}
			}
			temp_sigma.num_entries = num_entries;
			cusp::print(temp_sigma);
		}
		if(sigma.num_entries != sigma_prime.num_entries)
			sigma_change = true;
		else
			sigma_change = false;

		fprintf(stdout, "\nupdate r\n");
		if(r.size() == temp_r.size())
			thrust::transform(r.begin(), r.end(), r_prime.begin(), temp_r.begin(), thrust::minus<INDEX_TYPE>());
		// if(debug)
		// {
		// 	fprintf(stdout, "***r difference***\n");
		// 	cusp::print(temp_r);
		// 	cusp::print(r_prime);
		// }
		r_change = false;
		for(int i=0; i<temp_r.size(); ++i)
			if(temp_r[i] != 0)
			{
				r_change = true;
				break;
			}

		r = r_prime;
		sigma = sigma_prime;
		fprintf(stdout, "end iteration\n");

	} while(r_change || sigma_change);

	fprintf(stdout, "Analysis Complete...\n");
}

template <typename INDEX_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, MEM_TYPE>::WriteStore()
{
	// fprintf(stdout, "\nr:\n");
	// cusp::print(r);

	cusp::coo_matrix<int, INDEX_TYPE, MEM_TYPE> store;
	cusp::transpose(sigma, store);
	// fprintf(stdout, "\nsigma:\n");
	// cusp::print(store);

	std::ofstream output("tests/output.dat");
	output << "sigma " << store.num_rows << " " << store.num_cols << std::endl;
	for(int i=0; i<store.num_entries; ++i)
	{
		output << store.row_indices[i] << " " << store.column_indices[i] << " " << std::endl;
	}

	output.close();
}

void CPU_Test(std::string filename)
{
#if(BUILD_TYPE == CPU)
	CFA<int, cusp::host_memory> Analysis;
#elif(BUILD_TYPE == GPU)
	CFA<int, cusp::device_memory> Analysis;
	Analysis.gpuDeviceInit(1);
#endif	

	Analysis.ReadTestFile(filename.c_str());
	Analysis.Init();

	double startTime = omp_get_wtime();
	Analysis.Run_Analysis();
	double endTime = omp_get_wtime();

	fprintf(stdout, "Run Time: %f seconds\n", endTime - startTime);
	Analysis.WriteStore();
}

void GPU_Test()
{

}