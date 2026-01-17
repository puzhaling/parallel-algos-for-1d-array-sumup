#include	"barrier.h"


latch::latch (unsigned const pool_size)
	: pool_size (pool_size)
{
}

void
latch::arrive_and_wait ()
{
	std::unique_lock l{ mtx };

	if (++arrived < pool_size) 
		do allhere.wait (l); while (arrived < pool_size);
	else
		allhere.notify_all ();
}



barrier::barrier (unsigned const pool_size)
	: pool_size (pool_size)
{
}

void
barrier::arrive_and_wait ()
{
	std::unique_lock l{ mtx };

	if (++arrived < pool_size) {
		bool my_barrier = current_instance;
		do {
			allhere.wait (l);
		} while (my_barrier == current_instance);
	} else {
		arrived = 0;
		current_instance = !current_instance;
		allhere.notify_all ();
	}
}
