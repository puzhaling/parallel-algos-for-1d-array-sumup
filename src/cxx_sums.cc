#include	<vector>
#include	<thread>
#include	<mutex>

#include	"cxx_sums.h"
#include	"conf_thrd_pool.h"
#include	"barrier.h"


static std::pair<size_t, size_t>
get_subvector (size_t n, unsigned t, unsigned T);


/**
 *  @addtogroup sum_funcs
 *  @{  
 */

/*
 * st = [ down(n/T) + 1, t < n mod T
 *      [ down(n/T)
 *
 * i_start = [ t * down(n/T), t <= n mod T
 *           [ (n mod T) * (down(n/T)+1) + (t - n mod T)*down(n/T) = ...
 *           ... t * down(n/T) + (n mod T), t > n mod T
 * i_end = i_start + st
 */
unsigned
sum_loc (const unsigned *V, size_t n)
{
	unsigned T = get_num_threads ();
	std::vector<std::thread> workers (T-1);
	std::mutex mtx;
	unsigned END_RESULT = 0;
	
	auto worker_proc = ([V, n, T, &mtx, &END_RESULT](unsigned t) {
		size_t a = n % T, b = n / T;
		if (t < a)
			a = t * ++b;
		else
			a += t * b;
		b += a;

		unsigned res = 0;
		for (size_t i = a; i < b; ++i) {
			res += V[i];
		}
		
		{
			std::scoped_lock<std::mutex> slock (mtx);	// std::lock_guard for C++11.
			END_RESULT += res;
		}
	});

	for (unsigned t = 0; t < T-1; ++t)
		workers[t] = std::thread (worker_proc, t+1);

	worker_proc(0);

	for (unsigned t = 0; t < T-1; ++t)
		workers[t].join ();

	return END_RESULT;
}

unsigned
sum_loc_partial_sum (const unsigned *V, size_t n)
{
	unsigned T = get_num_threads ();
	std::vector<std::thread> workers (T-1);
	std::vector<partial_sum> partial_sums (T);
	unsigned END_RESULT = 0;
	
	auto worker_proc = ([V, n, T, &partial_sums](unsigned t) {
		size_t a = n % T, b = n / T;
		if (t < a)
			a = t * ++b;
		else
			a += t * b;
		b += a;
		
		unsigned res = 0;
		for (size_t i = a; i < b; ++i)
			res += V[i];

		partial_sums[t].v = res;
	});

	for (unsigned t = 0; t < T-1; ++t)
		workers[t] = std::thread (worker_proc, t+1);

	worker_proc(0);
	
	for (unsigned t = 0; t < T-1; ++t)
		workers[t].join ();
		
	for (unsigned i = 0; i < T; ++i)
		END_RESULT += partial_sums[i].v;

	return END_RESULT;
}

unsigned
sum_reduce (const unsigned *v, size_t n)
{
	unsigned T = get_num_threads();
	std::vector<std::thread> workers(T - 1);
	barrier bar{T};
	auto partial_sums = std::make_unique<partial_sum[]>(T);

	auto worker_proc = [v, n, T, &bar, &partial_sums](unsigned t) {
		auto [b, e] = get_subvector(n, t, T);
		unsigned r = 0;

		for (size_t i = b; i < e; ++i)
			r += v[i];

		partial_sums[t].v = r;

		// Reduce
		for (size_t step = 1, rem = 2; step < T; step = rem, rem *= 2) {
			bar.arrive_and_wait();
			if (t % rem == 0 && t + step < T)
				partial_sums[t].v += partial_sums[t + step].v;
		}
	};

	for (unsigned t = 0; t < T - 1; ++t)
	workers[t] = std::thread(worker_proc, t + 1);

	worker_proc(0);

	for (unsigned i = 0; i < T - 1; ++i)
		workers[i].join();

	return partial_sums[0].v;
}

/** @} */

static std::pair<size_t, size_t>
get_subvector (size_t n, unsigned t, unsigned T)
{
	size_t s, b, e;

	if (t < n % T)
		s = n / T + 1;
	else
		s = n / T;

	if (t <= n % T)
		b = t * (n / T + 1);
	else
		b = t * (n / T) + (n % T);

	e = b + s;

	return {b, e};
}
