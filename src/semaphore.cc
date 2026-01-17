#include	"semaphore.h"


Semaphore::Semaphore (unsigned max_thr)
	: max_threads (max_thr)
{
}

void
Semaphore::acquire () {
	std::unique_lock l{ m };
	++passed;
	while (passed >= max_threads)
		cv.wait (l);	
}

void
Semaphore::release () {
	std::unique_lock l{ m };
	--passed;
	cv.notify_one ();
}

