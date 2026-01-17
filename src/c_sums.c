#include	"c_sums.h"
#include	"conf_thrd_pool.h"

/**
 *  @addtogroup sum_funcs Summation functions
 *  @{
 */
unsigned sum_seq(const unsigned *V, size_t n)
{
	unsigned r = 0;
	for (size_t i = 0; i < n; ++i) {
		r += V[i];
	}
	return r;
}

unsigned sum_autoreduce(const unsigned *V, size_t n)
{
	unsigned r = 0;
	#pragma omp parallel for reduction (+:r)
	for (size_t i = 0; i < n; ++i) {
		r += V[i];
	}
	return r;
}

unsigned sum_rr_fs(const unsigned *V, size_t n)
{
	unsigned *partial, T; 

	#pragma omp parallel shared(T)
	{
		unsigned t = omp_get_thread_num(); 

		#pragma omp single
		{
			// T = omp_get_num_threads();
			T = get_num_threads();
			partial = (unsigned *) calloc(CACHE_SIZE, T);
		}

		for (size_t i = t; i < n; i += T) {
			partial[(t * CACHE_SIZE) / sizeof(unsigned)] += V[i];
		}
	}

	unsigned s = 0;
	
	for (size_t t = 0; t < T; ++t)
		s += partial[t * CACHE_SIZE / sizeof(unsigned)];

	free(partial);

	return s;
}

unsigned sum_rr_fs_partial_sum(const unsigned *V, size_t n)
{
	struct partial_sum *partial; unsigned T; 

	#pragma omp parallel
	{
		unsigned t = omp_get_thread_num();

		#pragma omp single
		{
			// T = omp_get_num_threads();
			T = get_num_threads();
			partial = (struct partial_sum *) calloc(T, sizeof(struct partial_sum));
		}

		unsigned nn = 0;
		for (size_t i = t; i < n; i += T)
			nn += V[i];

		partial[t].v = nn;
	}

	unsigned s = 0;
	
	for (size_t t = 0; t < T; ++t)
		s += partial[t].v;

	free(partial);

	return s;
}

/** @} */
