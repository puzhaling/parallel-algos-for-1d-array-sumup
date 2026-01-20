#ifndef CXX_SUMS_H
# define CXX_SUMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "c_sums.h"	// for struct partial_sum.

#ifdef __cplusplus
}
#endif

/**
 *  @addtogroup sum_funcs
 *  @{  
 */
unsigned sum_loc (const unsigned *V, size_t n);

unsigned sum_loc_partial_sum (const unsigned *V, size_t n);

unsigned sum_reduce (const unsigned *v, size_t n);
/** @} */

#endif /* !CXX_SUMS_H */

