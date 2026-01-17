#include <iostream>
#include <thread>
#include <vector>
#include <memory>


union partial_sum {
	alignas(64) unsigned v;
};


unsigned sum_fs (const unsigned* v, size_t n)
{
	unsigned result = 0;
	unsigned P = std::thread::hardware_concurrency();
	auto partial_sums = std::make_unique<partial_sum[]> (P);

	std::vector<std::thread> workers(P - 1);

	auto worker_proc = [&](unsigned t)
	{
		unsigned T = P;
		unsigned rr = 0;
		partial_sums[t].v = 0;

		for (size_t i = t; i < n; i += T)
			rr += v[i];

		partial_sums[t].v = rr;
	};

	{
		for (unsigned t = 0; t < P - 1; ++t)
			workers[t] = std::thread(worker_proc, t + 1);

		worker_proc(0);

		for (unsigned t = 0; t < P - 1; ++t)
			workers[t].join();

		for (size_t i = 0; i < P; ++i)
			result += partial_sums[i];
	}

	return result;
}
