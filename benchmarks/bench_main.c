/**
 * @file bench_main.c
 * @brief Benchmark runner entry point for stats module
 */

#include "bench_framework.h"
#include <platform.h>
#include <simd_detect.h>
#include <stdio.h>

/* External benchmark suites */
extern void bench_stats_run(void);

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    fc_init();

    /* CRITICAL: Detect SIMD level BEFORE any benchmarks run */
    fc_simd_level_t simd_level = fc_get_simd_level();

    fc_bench_init();

    printf("stats performance benchmarks v%s\n", FC_BENCH_VERSION);
    printf("SIMD level: %s\n", fc_simd_level_string(simd_level));
    printf("\n");

    bench_stats_run();

    fc_bench_cleanup();
    return 0;
}
