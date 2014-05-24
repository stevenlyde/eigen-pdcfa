#ifndef SCAN_H
#define SCAN_H

#define NUM_BANKS 		32
#define LOG_NUM_BANKS	5
#define CONFLICT_FREE_OFFSET(n)		(n >> LOG_NUM_BANKS)

template<typename T>
__device__ void prescanE(T *g_odata, T *g_idata, T *temp, const int n)
{
	int tID = threadIdx.x;  
	int offset = 1;
	
	int ai = tID;  							// load input into shared memory  
	int bi = tID + (n/2);  
	int bankOffsetA = CONFLICT_FREE_OFFSET(ai);
	int bankOffsetB = CONFLICT_FREE_OFFSET(bi);
	temp[ai + bankOffsetA] = g_idata[ai];
	temp[bi + bankOffsetB] = g_idata[bi];
	 	
	for(int d = n>>1; d > 0; d >>= 1)		// build sum in place up the tree  
	{
		__syncthreads();  
		if(tID < d)
		{
			int ai = offset*(2*tID+1)-1;  
			int bi = offset*(2*tID+2)-1;  
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);

			temp[bi] += temp[ai]; 
		}
		offset <<= 1;
	}

	if(tID==0)
	{ temp[n - 1 + CONFLICT_FREE_OFFSET(n - 1)] = 0; }		// clear the last element
	 	
	for(int d = 1; d < n; d *= 2)			// traverse down tree & build scan  
	{
		offset >>= 1;
		__syncthreads();  

		if(tID < d)                  
		{
			int ai = offset*(2*tID+1)-1;  
			int bi = offset*(2*tID+2)-1;
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);

			T t = temp[ai];  
			temp[ai] = temp[bi];  
			temp[bi] += t;   
		}
	}
	__syncthreads();

	g_odata[ai] = temp[ai + bankOffsetA];	// write results to device memory  
	g_odata[bi] = temp[bi + bankOffsetB];
}

template<typename T>
__device__ void prescanE(T *data, const int n)
{
	int tID = threadIdx.x;  
	int offset = 1;
	 	
	for(int d = n>>1; d > 0; d >>= 1)		// build sum in place up the tree  
	{
		__syncthreads();  
		if(tID < d)
		{
			int ai = offset*(2*tID+1)-1;  
			int bi = offset*(2*tID+2)-1;  
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);

			data[bi] += data[ai]; 
		}
		offset <<= 1;
	}

	if(tID==0)
	{ data[n - 1 + CONFLICT_FREE_OFFSET(n - 1)] = 0; }		// clear the last element
	 	
	for(int d = 1; d < n; d *= 2)			// traverse down tree & build scan  
	{
		offset >>= 1;
		__syncthreads();  

		if(tID < d)                  
		{
			int ai = offset*(2*tID+1)-1;  
			int bi = offset*(2*tID+2)-1;
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);

			T t = data[ai];  
			data[ai] = data[bi];  
			data[bi] += t;   
		}
	}
	__syncthreads();
}

template<typename T>
__device__ void prescanI(T *data, const int n)
{
	int tID = threadIdx.x;  
	int offset = 1;
	 	
	for(int d = n>>1; d > 0; d >>= 1)		// build sum in place up the tree  
	{
		__syncthreads();
		if(tID < d)
		{
			int ai = offset*(2*tID+1)-1;
			int bi = offset*(2*tID+2)-1;
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);

			data[bi] += data[ai]; 
		}
		offset <<= 1;
	}

	offset >>= 1;
	for(int d = 2; d < n; d *= 2)			// traverse down tree & build scan  
	{
		offset >>= 1;
		__syncthreads();

		if(tID < d-1)
		{
			int ai = offset*(2*tID+2)-1;  
			int bi = offset*(2*tID+3)-1;
			ai += CONFLICT_FREE_OFFSET(ai);
			bi += CONFLICT_FREE_OFFSET(bi);
  
			data[bi] += data[ai];   
		}
	}
	__syncthreads();
}

#endif