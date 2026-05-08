/**
 * @file bench_stats.c
 * @brief Benchmark suite runner for stats module
 */

#include "bench_framework.h"

/* External benchmark runner functions */
extern void bench_mean_var_run(void);
extern void bench_kahan_sum_run(void);
extern void bench_welford_run(void);
extern void bench_covariance_run(void);
extern void bench_quantile_run_all(void);
extern void bench_rolling_run(void);
extern void bench_rank_run(void);
extern void bench_ema_run(void);
extern void bench_moments_run(void);

/**
 * @brief Run all stats benchmarks
 */
void bench_stats_run(void) {
    bench_mean_var_run();
    bench_kahan_sum_run();
    bench_welford_run();
    bench_covariance_run();
    bench_quantile_run_all();
    bench_rolling_run();
    bench_rank_run();
    bench_ema_run();
    bench_moments_run();
}
