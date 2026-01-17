#include	<omp.h>
#include	<chrono>
#include	<iostream>
#include	<vector>
#include	<thread>
#include	<iomanip>
#include	"cxx_sums.h"
#include	"conf_thrd_pool.h"

extern "C" {
	#include "c_sums.h"
}

/**
 *  @brief Used as a shortcut to initialize @ref sum_command structure
 */
#define SUM_COMMAND(NAME)   { #NAME, sum_ ## NAME }


/**
 *  @brief All summation function used in the project have this prototype
 */
typedef unsigned (*sumfunc)(const unsigned *, size_t);

/**
 *  @brief Used to associate a function with its name
 */
struct sum_command {
	const char *name;  /**< Function name  */
	sumfunc func;      /**< Function */
};

/**
 *  @brief Contains the results of the experiment
 */
struct exp_result {
	double time;         /**< Time spent on the experiment */
	double speedUp;      /**< A coefficient that determines the
	                          increase in speed compared to the initial
						      number of threads */
	double efficiency;   /**< The ratio of acceleration to the number of
	                          threads (ideally must be close to 1) */
	unsigned T;          /**< Number of threads used to conduct the experiment */
	unsigned result;     /**< Result of the summation */
};

/**
 *  @addtogroup auxiliary_funcs Auxiliary functions
 *  @brief All auxiliary function used in the project
 *
 *  @{
 */

/**
 *  @brief Generates a vector of size @b n filled with
 *         consecutive values from 1 to n
 */
auto
generate (size_t n)
{
	std::vector<unsigned> v(n);

	for (size_t i = 0; i < n; ++i)
		v[i] = i;

	return v;
}

/**
 *  @brief Heart of the project
 *
 *  This function performs all necessary performance measurements
 *
 *  @param f              Function under test
 *  @param problem_size   The size of the vector we generate and the
 *                        elements of which we sum
 *  @returns Results of the experiment
 */
std::vector<exp_result>
conduct_experiment (sumfunc f, const size_t problem_size)
{
	using namespace std::chrono;

	auto v = generate (problem_size);
	unsigned T = get_num_threads ();
	std::vector<exp_result> results (T);

	set_num_threads (1);
	auto t0 = steady_clock::now();
	auto base_result = f (v.data(), v.size());
	auto t1 = steady_clock::now();
	double base_time = duration_cast<milliseconds> (t1 - t0).count();
	results[0] = {
		.time = base_time,
		.speedUp = 1.0,
		.efficiency = 1.0,
		.T = (unsigned )1,
		.result = base_result,
	};

	
	for (unsigned threads = 2; threads <= T; ++threads) {
		// omp_set_num_threads (threads);
		set_num_threads (threads);

		v = generate (problem_size);
		
		t0 = steady_clock::now();
		auto result = f (v.data(), v.size());
		t1 = steady_clock::now();

		double time = duration_cast<milliseconds> (t1 - t0).count();
		double speedUp = base_time / time;
		double efficiency = speedUp / threads;
		
		results[threads-1] = {
			.time = time,
			.speedUp = speedUp,
			.efficiency = efficiency,
			.T = (unsigned) threads,
			.result = result,
		};
	}
	
	return results;
}

/**
 *  @brief Outputs the results @b results of the function @b name involved
 *         in the experiment in CSV format
 *
 *  @param name     The name of the function used in the experiment
 *  @param results  @b name function results
 */
void
print_csv_results (const std::vector<exp_result> &results, const char* name)
{
	for (const auto &r : results) {
		std::cout << name << ","
			<< r.T << ","
			<< r.time << ","
			<< r.speedUp << ","
			<< r.efficiency << ",0x" <<
			std::hex << r.result << "\n";
	}	
}
/** @} */

int
main ()
{
	constexpr size_t N = 1 << 22;
	
	unsigned sum_loc (const unsigned *V, size_t n);
	struct sum_command const sum_commands[] = {
		SUM_COMMAND (seq),
		SUM_COMMAND (autoreduce),
		SUM_COMMAND (rr_fs),
		SUM_COMMAND (rr_fs_partial_sum),
		SUM_COMMAND (loc),
		SUM_COMMAND (loc_partial_sum)
	};

	std::cout << "Algorithm,Nthreads,Time(ms),SpeedUp,Efficiency,Result\n";
	for (size_t cmdi = 0; cmdi <
			sizeof (sum_commands) / sizeof (struct sum_command); ++cmdi)
	{
		auto results = conduct_experiment (sum_commands[cmdi].func, N);
		print_csv_results (results, sum_commands[cmdi].name);
	}
}

