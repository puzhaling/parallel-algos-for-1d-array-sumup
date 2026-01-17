#include	"conf_thrd_pool.h"
#include	<omp.h>

/**
 *  @brief Currently set number of threads
 */
static unsigned  _thread_pool_size = omp_get_num_procs ();


unsigned get_num_threads ()
{
	return _thread_pool_size;
}

unsigned set_num_threads (unsigned nthreads)
{
	unsigned prev_size = _thread_pool_size;
	
	_thread_pool_size = nthreads;
	omp_set_num_threads (nthreads);

	return prev_size;
}
