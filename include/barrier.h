#ifndef BARRIER_H
# define BARRIER_H


#include	<condition_variable>
#include	<mutex>


class latch
{
public:
	latch (unsigned const pool_size);

	void arrive_and_wait ();

private:
	unsigned arrived;
	std::condition_variable allhere;
	unsigned const pool_size;
	std::mutex mtx;
};


class barrier
{
public:
	barrier (unsigned const pool_size);

	void arrive_and_wait ();

private:
	bool current_instance = true;
	unsigned arrived = 0;
	std::condition_variable allhere;
	unsigned const pool_size;
	std::mutex mtx;
};


#endif  /* !BARRIER_H */
