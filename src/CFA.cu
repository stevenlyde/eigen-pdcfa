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

// template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
// inline void subtract_vec(cusp::array1d<INDEX_TYPE, MEM_TYPE> &A,
// 							cusp::array1d<INDEX_TYPE, MEM_TYPE> &B,
// 							cusp::array1d<INDEX_TYPE, MEM_TYPE> &C)
// {
// 	thrust::transform(A.begin(), A.end(), B.begin(), C.begin(), thrust::minus<INDEX_TYPE>());
// }

// General GPU Device CUDA Initialization
// int gpuDeviceInit(int devID)
// {
//     int deviceCount;
//     checkCudaErrors(cudaGetDeviceCount(&deviceCount));

//     if (deviceCount == 0)
//     {
//         fprintf(stdout, "gpuDeviceInit() CUDA error: no devices supporting CUDA.\n");
//         exit(-1);
//     }

//     if (devID < 0)
//        devID = 0;
        
//     if (devID > deviceCount-1)
//     {
//         fprintf(stdout, "\n");
//         fprintf(stdout, ">> %d CUDA capable GPU device(s) detected. <<\n", deviceCount);
//         fprintf(stdout, ">> gpuDeviceInit (-device=%d) is not a valid GPU device. <<\n", devID);
//         fprintf(stdout, "\n");
//         return -devID;
//     }

//     cudaDeviceProp deviceProp;
//     checkCudaErrors( cudaGetDeviceProperties(&deviceProp, devID) );

//     if (deviceProp.major < 1)
//     {
//         fprintf(stdout, "gpuDeviceInit(): GPU device does not support CUDA.\n");
//         exit(-1);
//     }
    
//     checkCudaErrors( cudaSetDevice(devID) );
//     checkCudaErrors( cudaDeviceSetCacheConfig(cudaFuncCachePreferL1) );
//     printf("gpuDeviceInit() CUDA Device [%d]: \"%s\n", devID, deviceProp.name);

//     return devID;
// }

int * shared_store::host_column_indices_UA = NULL;
int * shared_store::host_values_UA = NULL;
int * shared_store::host_column_indices = NULL;
int * shared_store::host_values = NULL;
size_t shared_store::num_rows = 0;
size_t shared_store::num_cols = 0;
size_t shared_store::num_cols_per_row = 0;
size_t shared_store::pitch = 0;
size_t shared_store::num_entries = 0;

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::Init()
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
	AND_vec1.resize(r.size(), 0);
	AND_vec2.resize(r.size(), 0);

	for(int i=0; i<ARG_MAX; ++i)
	{
		v[i].resize(vec_size, 0);
		a[i].resize(sigma.num_cols, 0);
		
		if(Call[i].size() != r.size())
			Call[i].resize(r.size(), 0);
		if(PrimList[i].size() != r.size())
			PrimList[i].resize(r.size(), 0);
	}
#if(BUILD_TYPE == GPU)
	for(int i=0; i<NUM_STREAMS; ++i)
	{
		a_var[i].resize(sigma.num_cols, 0);
	    vf[i].resize(vec_size, 0);
	}
#else
	a_var.resize(sigma.num_cols, 0);
	vf.resize(vec_size, 0);
#endif
    a_set.resize(sigma.num_cols, 0);
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
    cudaStreamCreate(&stream_Call);
    cudaStreamCreate(&stream_List);
    cudaStreamCreate(&stream_Set);
    cudaStreamCreate(&stream_IF);
    cudaStreamCreate(&stream_Num);
    cudaStreamCreate(&stream_Bool);
    cudaStreamCreate(&stream_Void);

	a_indices.resize(sigma.num_cols, 0);
	v_indices.resize(vec_size, 0);
	for(int i=0; i<NUM_STREAMS; ++i)
	{
		index_count[i].resize(8, 0);
		temp_row_indices[i].resize(vec_size, 0);
		temp_col_indices[i].resize(sigma.num_cols, 0);
		s[i].resize(r.size(), 0);
		s_indices[i].resize(r.size(), 0);
		Fun_vec[i].resize(Fun.num_rows, 0);
		Body_vec[i].resize(Body.num_rows, 0);
		Arg_vec[i].resize(Arg[0].num_rows, 0);
		accum_var_vec[i].resize(sigma.num_cols);
		accum_vf_vec[i].resize(vec_size);
	}
	Cond_vec.resize(CondTrue.num_rows);
	for(int i=0; i<4*NUM_STREAMS; ++i)
		temp_Mat[i].resize(sigma.num_rows, sigma.num_cols, 0, std::max(sigma.num_cols/20, ulong(32)), 32);
	#else
	s.resize(r.size(), 0);
	s_indices.resize(r.size(), 0);
	#endif
}

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::ReadTestFile(const char* filename)
{
	int ID = omp_get_thread_num();

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
		valid_Call.resize(ARG_MAX);
		valid_List.resize(ARG_MAX);

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

			//check name
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

			cusp::coo_matrix<int, int, cusp::host_memory> A(rows, cols, indices.size());
			cusp::coo_matrix<int, int, cusp::host_memory> B(cols, rows, indices.size());

			for(int i=0; i<indices.size(); ++i)
			{
				A.row_indices[i] = indices[i].first;
				A.column_indices[i] = indices[i].second;
				A.values[i] = 1;
			}
			A.sort_by_row_and_column();
			cusp::transpose(A, B);
			B.sort_by_row_and_column();

			cusp::array1d<int, cusp::host_memory> vec;
			if(A.num_cols == 1)
			{
				vec.resize(A.num_rows, 0);
				for(int i=0; i<A.num_entries; ++i)
				{
					vec[A.row_indices[i]] = 1;
				}
			}

			if(ID == 0 && A.num_entries > 0)
			{
				fprintf(stderr, "\n%s (%d x %d) with %d entries\n", name, A.num_rows, A.num_cols, A.num_entries);
				fprintf(stderr, "B: (%d x %d)\n", B.num_rows, B.num_cols);
			}

			//parse name
			if(sname == "r")
				r = vec;
			else if(sname == "sigma")
			{
#if BUILD_TYPE == CPU
				sigma = B;
#else			
				if(ID == 0)
				{
					shared_sigma.num_rows = B.num_rows;
					shared_sigma.num_cols = B.num_cols;
					shared_sigma.num_cols_per_row = std::max(B.num_cols/20, ulong(32));
					shared_sigma.pitch = B.num_rows;
					shared_sigma.num_entries = B.num_entries;

					size_t index_size = shared_sigma.pitch  * shared_sigma.num_cols_per_row * sizeof(INDEX_TYPE);
					size_t values_size = shared_sigma.pitch * shared_sigma.num_cols_per_row * sizeof(VALUE_TYPE);

					// checkCudaErrors(cudaHostAlloc((void **)&shared_sigma.host_column_indices, index_size, cudaHostAllocMapped));
					// checkCudaErrors(cudaHostAlloc((void **)&shared_sigma.host_values, values_size, cudaHostAllocMapped));

					size_t entry_count_size = 32*sizeof(INDEX_TYPE);
					checkCudaErrors( cudaHostAlloc((void **)&entry_count_host, entry_count_size, 0));
					checkCudaErrors( cudaMalloc((void **)&entry_count_device, entry_count_size));
					memset(entry_count_host, 0, entry_count_size);
					checkCudaErrors( cudaMemcpy(entry_count_device, entry_count_host, entry_count_size, cudaMemcpyHostToDevice) );
				}
				#pragma omp barrier
				//checkCudaErrors(cudaHostGetDevicePointer((void **)&shared_sigma.column_indices, (void *)shared_sigma.host_column_indices, 0));
				//checkCudaErrors(cudaHostGetDevicePointer((void **)&shared_sigma.values, (void *)shared_sigma.host_values, 0));
				//checkCudaErrors(cudaHostGetDevicePointer((void **)&entry_count_device, (void *)entry_count_host, 0));
				#pragma omp barrier
				cusp::csr_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> temp;
				temp = B;
				LoadEllMatrix(temp, sigma);
				int num_entries = thrust::count_if(sigma.column_indices.values.begin(), sigma.column_indices.values.end(), is_non_negative());
				fprintf(stderr, "num_entries: %d\n", num_entries);

				// LoadEllMatrix(temp, shared_sigma);
				// fprintf(stderr, "shared_sigma.num_cols: %d\n", shared_sigma.num_cols);
				// fprintf(stderr, "shared_sigma.num_rows: %d\n", shared_sigma.num_rows);
#endif
				if(ID == 0)
					print_matrix_info(sigma);
			}
			else if(sname == "CondTrue")
			{
				CondTrue = B;
				if(ID == 0)
					print_matrix_info(CondTrue);
			}
			else if(sname == "CondFalse")
			{
				CondFalse = B;
				if(ID == 0)
					print_matrix_info(CondFalse);
			}
			else if(sname == "Body")
			{
				Body = B;
				if(ID == 0)
					print_matrix_info(Body);
			}
			else if(sname == "Fun")
			{
				Fun = B;
				if(ID == 0)
					print_matrix_info(Fun);
			}
			else if(sname == "Arg")
			{
				Arg[mat_num-1] = B;
				if(ID == 0)
					print_matrix_info(Arg[mat_num-1]);
			}
			else if(sname == "Var")
			{
				Var[mat_num-1] = B;
				if(ID == 0)
					print_matrix_info(Var[mat_num-1]);
			}
			else if(sname == "Call")
			{
				if(A.num_entries > 0)
					valid_Call[mat_num] = true;
				else
					valid_Call[mat_num] = false;

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
				if(A.num_entries > 0)
					valid_List[mat_num] = true;
				else
					valid_List[mat_num] = false;

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

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::Run_Analysis()
{
	int ID = omp_get_thread_num();
	fprintf(stdout, "\n\n\nStarting analysis: %d\n", ID);

	int prev_num_entries = sigma.num_entries;
	r_prime = r;
	int iter=0;
	bool sigma_change = false, r_change = false;
	fprintf(stderr, "m_maxCall: %d  m_maxList: %d\n", m_maxCall, m_maxList);

	//#pragma omp parallel num_threads(NUM_STREAMS)
	do
	{
		iter++;
		if(ID == 0)
			fprintf(stdout, "\n\nITERATION %d\n\n", iter);

		// if(iter == 12)
		// 	debug = true;
		// else
		// 	debug = false;

#if BUILD_TYPE == GPU
		//if(ID == 1)
		 	f_call();
		//if(ID == 2)
			f_list();
		//if(ID == 3)
			f_set();
		//if(ID == 4)
			f_if();
		//if(ID == 5)
		 	f_primBool();
		//if(ID == 6)
			f_primNum();
		//if(ID == 7)
			f_primVoid();
#else
		f_call();
		f_list();
		f_set();
		f_if();
	 	f_primBool();
	 	f_primNum();
		f_primVoid();
#endif

	//#pragma omp barrier
		if(ID == 0 && iter % 5 == 0)
		{
			fprintf(stdout, "\nupdate sigma\n");
		#if BUILD_TYPE == GPU
			// #if MULTI_GPU == 2
			// int count = 0;
			// for(int row=0; row<shared_sigma.num_rows; ++row)
			// {
			// 	int offset = row;
			// 	for(int col=0; col < shared_sigma.num_cols_per_row; ++col, offset+=shared_sigma.pitch)
			// 	{
			// 		if(shared_sigma.column_indices[offset] != -1)
			// 			count++;
			// 	}
			// }
			// shared_sigma.num_entries = count;
			// #else
			if(iter % 5 == 0)
			sigma.num_entries = thrust::count_if(sigma.column_indices.values.begin(), sigma.column_indices.values.end(), is_non_negative());
			fprintf(stderr, "num_entries: %d  %d\n", prev_num_entries, sigma.num_entries);
			//DEBUG_PRINT("sigma", sigma);

			//#endif
		#else
			//sigma.num_entries = thrust::count_if(sigma.column_indices.values.begin(), sigma.column_indices.values.end(), is_non_negative());
			//thrust::fill(sigma.values.begin(), sigma.values.end(), 1);
		#endif

			if(prev_num_entries != sigma.num_entries)
				sigma_change = true;
			else
				sigma_change = false;
			prev_num_entries = sigma.num_entries;

			fprintf(stdout, "\nupdate r\n");
			int r_entries = thrust::count(r.begin(), r.end(), 1);
			int r_prime_entries = thrust::count(r_prime.begin(), r_prime.end(), 1);

			if(r_entries != r_prime_entries)
				r_change = true;
			else
				r_change = false;

			r = r_prime;
			fprintf(stderr, "sigma.num_entries: %d\n", sigma.num_entries);
			//sigma = sigma_prime;
			fprintf(stdout, "end iteration\n");
		}
		else
		{
			r_change = true;
			sigma_change = true;
		}

	//#pragma omp barrier
	} while(r_change || sigma_change);

	fprintf(stdout, "Analysis Complete...\n");
}

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
void CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::WriteStore()
{
	// fprintf(stdout, "\nr:\n");
	// cusp::print(r);

#if BUILD_TYPE == GPU
	int num_entries = CountEntries(sigma);
	sigma.num_entries = num_entries;
#endif

	cusp::coo_matrix<int, VALUE_TYPE, cusp::host_memory> store;
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

void Test(std::string filename)
{
	double startTime, endTime;
#if BUILD_TYPE == CPU
	CFA<int, int, cusp::host_memory> Analysis;

	Analysis.ReadTestFile(filename.c_str());
	Analysis.Init();

	startTime = omp_get_wtime();
	Analysis.Run_Analysis();
	endTime = omp_get_wtime();

	fprintf(stdout, "Run Time: %f seconds\n", endTime - startTime);
	Analysis.WriteStore();

#elif BUILD_TYPE == GPU
	#pragma omp parallel num_threads(NUM_GPUS)
	{
		int ID = omp_get_thread_num();
		gpuDeviceInit(ID);
		fprintf(stderr, "thread ID: %d\n", ID);
		CFA<int, int, cusp::device_memory> Analysis;

		Analysis.ReadTestFile(filename.c_str());
		Analysis.Init();

	#pragma omp barrier

		if(ID == 0)
			startTime = omp_get_wtime();
		
		Analysis.Run_Analysis();

		if(ID == 0)
		{
			endTime = omp_get_wtime();
			fprintf(stdout, "Run Time: %f seconds\n", endTime - startTime);
			Analysis.WriteStore();
		}
	}
#endif

	
}

template <typename INDEX_TYPE, typename VALUE_TYPE, typename MEM_TYPE>
int CFA<INDEX_TYPE, VALUE_TYPE, MEM_TYPE>::CountEntries(cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::device_memory> &mat)
{
	cusp::ell_matrix<INDEX_TYPE, VALUE_TYPE, cusp::host_memory> temp(mat);
	const INDEX_TYPE invalid_index = cusp::ell_matrix<int, INDEX_TYPE, cusp::device_memory>::invalid_index;

	int num_entries = 0;
	int pitch = temp.column_indices.pitch;
	for(int col=0; col<temp.column_indices.num_cols; ++col)
	{
		int offset = pitch*col;
		for(int row=0; row<temp.num_rows; ++row, ++offset)
		{
			if(temp.column_indices.values[offset] != invalid_index)
				num_entries++;
		}
	}
	temp.num_entries = num_entries;
	//cusp::print(temp);

	return num_entries;
}