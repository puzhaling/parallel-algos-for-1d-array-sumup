#ifndef CONF_THRD_POOL_H 
#  define CONF_THRD_POOL_H

/**
 *  @file
 *
 *  @brief Universal interface for configuring the number of threads
 *
 *  This file describes a generic interface that allows you to set
 *  and get the number of specified threads
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup auxiliary_funcs
 *  @{
 */

/**
 *  @brief Get number of threads
 *  @returns Сurrently set number of threads
 */
unsigned get_num_threads ();

/**
 *  @brief Set number of threads
 *
 *  @param nthreads Number of threads to set
 *  @returns Last set number of threads
 */
unsigned set_num_threads (unsigned nthreads);

/** @} */

#ifdef __cplusplus
}
#endif


#endif  /*  !CONF_THRD_POOL_H  */
