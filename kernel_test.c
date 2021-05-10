#define CLRNG_SINGLE_PRECISION
#include "clRNG/philox.h"
#include "limits.h"

#define FACTOR 1.f / ((float)UINT_MAX + 1.f)
#define HALF   0.5f * FACTOR
#define REDUX_BLOCK \
	sdata[lid] = current_sum;\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 512) && (lid < 256))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 256];\
	barrier(CLK_LOCAL_MEM_FENCE);\
	\
	if ((get_local_size(0) >= 256) && (lid < 128))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 128];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 128) && (lid < 64))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 64];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 64) && (lid < 32))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 32];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 32) && (lid < 16))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 16];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 16) && (lid < 8))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 8];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 8) && (lid < 4))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 4];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 4) && (lid < 2))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 2];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if ((get_local_size(0) >= 2) && (lid < 1))\
		sdata[lid] = current_sum = current_sum + sdata[lid + 1];\
	barrier(CLK_LOCAL_MEM_FENCE);\
\
	if (lid == 0) {\
		ogdata[get_group_id(0)] = current_sum;\
	}

inline float2 box_muller(const float u1, const float u2) {
	float2 res;
	res.x = sqrt(-2.f*log(u1))*cospi(2.f*u2);
	res.y = sqrt(-2.f*log(u1))*sinpi(2.f*u2);

	return res;
}

void kernel bachelier_vanilla(global size_t* counter,
				   global float* S,
				   global const float* S0,
				   global const float* S0_sigbar,
				   global const int* nbr_timesteps)
	{
		const int gid = get_global_id(0);
		philox4x32_key_t k = {{gid, 1}};
		philox4x32_ctr_t c = {{counter[gid]}};

		const float S0_sigbar_private = *S0_sigbar;
		const int nbr_timesteps_private = (*nbr_timesteps)/4;

		float S_local = *S0;
		for (int t=0; t<nbr_timesteps_private; ++t) {
			union {
				philox4x32_ctr_t c;
				uint4 i;
			} u;
			c.v[0]++;
			u.c = philox4x32(c, k);
			float4 G = convert_float4(u.i) * FACTOR; 
			G.x += HALF;
			G.y += HALF;
			G.z += HALF;
			G.w += HALF;
			float2 z = box_muller(G.x, G.y);
			float2 w = box_muller(G.z, G.w);
			
			S_local = S_local + (S0_sigbar_private)*(z.x + z.y + w.x + w.y);
		}

		S[gid] = S_local;
		counter[gid] = c.v[0];
	}

void kernel vanilla_call(global float* igdata,
			 global float* ogdata,
			 local float* sdata, 
			 const int n, 
			 const float K) 
{
	const unsigned int lid = get_local_id(0);
	unsigned int i = get_local_size(0) * get_group_id(0) * 2 + lid;
	const unsigned int grid_size = get_local_size(0) * get_num_groups(0) * 2;
	
	float current_sum = 0.f;
	float S, S1;

	while (i<n) {
		S = igdata[i];
		if (S-K>0)
			current_sum += S-K;
		if (i + get_local_size(0) < n) {
			S1 = igdata[i + get_local_size(0)];
			if (S1-K>0)
				current_sum += S1-K;
		}
		i += grid_size;
	}
	
	REDUX_BLOCK;
}

void kernel vanilla_call_squared(global float* igdata,
			 	 global float* ogdata,
			 	 local float* sdata, 
			 	 const int n, 
			 	 const float K) 
{
	const unsigned int lid = get_local_id(0);
	unsigned int i = get_local_size(0) * get_group_id(0) * 2 + lid;
	const unsigned int grid_size = get_local_size(0) * get_num_groups(0) * 2;
	
	float current_sum = 0.f;
	float S, S1;

	while (i<n) {
		S = igdata[i];
		if (S-K>0)
			current_sum += (S-K)*(S-K);
		if (i + get_local_size(0) < n) {
			S1 = igdata[i + get_local_size(0)];
			if (S1-K>0)
				current_sum += (S1-K)*(S1-K);
		}
		i += grid_size;
	}
	
	REDUX_BLOCK;
}

void kernel vanilla_put(global float* igdata,
			 global float* ogdata,
			 local float* sdata, 
			 const int n, 
			 const float K) 
{
	const unsigned int lid = get_local_id(0);
	unsigned int i = get_local_size(0) * get_group_id(0) * 2 + lid;
	const unsigned int grid_size = get_local_size(0) * get_num_groups(0) * 2;
	
	float current_sum = 0.f;
	float S, S1;

	while (i<n) {
		S = igdata[i];
		if (S-K<0)
			current_sum += K-S;
		if (i + get_local_size(0) < n) {
			S1 = igdata[i + get_local_size(0)];
			if (S1-K<0)
				current_sum += K-S1;
		}
		i += grid_size;
	}
	
	REDUX_BLOCK;
}

void kernel vanilla_put_squared(global float* igdata,
			 	global float* ogdata,
			 	local float* sdata, 
			 	const int n, 
			 	const float K) 
{
	const unsigned int lid = get_local_id(0);
	unsigned int i = get_local_size(0) * get_group_id(0) * 2 + lid;
	const unsigned int grid_size = get_local_size(0) * get_num_groups(0) * 2;
	
	float current_sum = 0.f;
	float S, S1;

	while (i<n) {
		S = igdata[i];
		if (S-K<0)
			current_sum += (K-S)*(K-S);
		if (i + get_local_size(0) < n) {
			S1 = igdata[i + get_local_size(0)];
			if (S1-K<0)
				current_sum += (K-S1)*(K-S1);
		}
		i += grid_size;
	}
	
	REDUX_BLOCK;
}

void kernel reduction(global float* igdata,
		      global float* ogdata,
		      local float* sdata,
		      const int n)
{
	const unsigned int lid = get_local_id(0);
	unsigned int i = get_local_size(0) * get_group_id(0) * 2 + lid;
	const unsigned int grid_size = get_local_size(0) * get_num_groups(0) * 2;

	float current_sum = 0.f;

	while (i<n) {
		current_sum += igdata[i];;
		if (i + get_local_size(0) < n) {
			current_sum += igdata[i + get_local_size(0)];
		}
		i += grid_size;
	}

	REDUX_BLOCK;
}
