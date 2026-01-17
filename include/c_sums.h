#ifndef C_SUMS_H
# define C_SUMS_H

#include	<omp.h>
#include	<strings.h>
#include 	<stdlib.h>
#include	<stddef.h>

/**
 *  @addtogroup sum_funcs
 *  @brief All summation function used in the project
 *
 *  @{  
 */
 
/**
 *  @brief Estimated cache line size
 */
#define CACHE_SIZE 64

struct partial_sum {
	unsigned v;
	char __padding[CACHE_SIZE - sizeof(unsigned)];
};

unsigned sum_seq(const unsigned *V, size_t n);

unsigned sum_autoreduce(const unsigned *V, size_t n);

unsigned sum_rr_fs(const unsigned *V, size_t n);

unsigned sum_rr_fs_partial_sum(const unsigned *V, size_t n);
/** @} */

#endif /* !C_SUMS_H */
