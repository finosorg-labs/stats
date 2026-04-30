/**
 * @file bench_stats.c
 * @brief Benchmark suite runner for stats module
 */

#include "bench_framework.h"

/* External benchmark runner functions */
extern void bench_mean_var_run(void);
extern void bench_kahan_sum_run(void);
extern void bench_welford_run(void);

/**
 * @brief Run all stats benchmarks
 */
void bench_stats_run(void) {
    bench_mean_var_run();
    bench_kahan_sum_run();
    bench_welford_run();
}
