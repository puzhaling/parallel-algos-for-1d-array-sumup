#include	<mutex>
#include	<condition_variable>

class Semaphore
{
public:
	Semaphore (unsigned max_thr);
	
	void acquire ();
	void release ();

private:
	unsigned const max_threads;
	unsigned passed = 0;
	std::mutex m;
	std::condition_variable cv;
};

