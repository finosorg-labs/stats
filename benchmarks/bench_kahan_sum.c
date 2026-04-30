/**
 * @file bench_kahan_sum.c
 * @brief Benchmark tests for Kahan compensated summation
 */

#include "bench_framework.h"
#include "kahan_sum.h"
#include "../../platform/include/simd_detect.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Benchmark data sizes */
#define SMALL_SIZE   100
#define MEDIUM_SIZE  10000
#define LARGE_SIZE   1000000

/* Naive summation for comparison */
static double naive_sum(const double* data, size_t n) {
    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        sum += data[i];
    }
    return sum;
}

/* Helper to generate random data */
static void generate_random_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)rand() / RAND_MAX * 1000.0 - 500.0;
    }
}

/* Benchmark functions */
static void bench_kahan_sum_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n = (size_t)(uintptr_t)data_ptr[1];
    volatile double result = fc_stat_kahan_sum_f64(data, n);
    (void)result;
}

static void bench_naive_sum_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n = (size_t)(uintptr_t)data_ptr[1];
    volatile double result = naive_sum(data, n);
    (void)result;
}

static void bench_kahan_state_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n = (size_t)(uintptr_t)data_ptr[1];
    fc_kahan_state_t state;
    fc_stat_kahan_init(&state);
    fc_stat_kahan_add_batch(&state, data, n);
    volatile double result = fc_stat_kahan_get_sum(&state);
    (void)result;
}

/* Run benchmarks for different sizes */
static void run_kahan_benchmarks(void) {
    printf("\nKahan Sum Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"KahanSum/Small/100", SMALL_SIZE},
        {"KahanSum/Medium/10K", MEDIUM_SIZE},
        {"KahanSum/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        if (!data) continue;

        generate_random_data(data, tests[i].size);

        void* user_data[2] = {data, (void*)(uintptr_t)tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_kahan_sum_fn, user_data, &result);

        free(data);
    }
}

/* Run naive sum benchmarks for comparison */
static void run_naive_benchmarks(void) {
    printf("\nNaive Sum Benchmarks (for comparison)\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"NaiveSum/Small/100", SMALL_SIZE},
        {"NaiveSum/Medium/10K", MEDIUM_SIZE},
        {"NaiveSum/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        if (!data) continue;

        generate_random_data(data, tests[i].size);

        void* user_data[2] = {data, (void*)(uintptr_t)tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_naive_sum_fn, user_data, &result);

        free(data);
    }
}

/* Run state-based benchmarks */
static void run_state_benchmarks(void) {
    printf("\nKahan State-Based Sum Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"KahanState/Small/100", SMALL_SIZE},
        {"KahanState/Medium/10K", MEDIUM_SIZE},
        {"KahanState/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        if (!data) continue;

        generate_random_data(data, tests[i].size);

        void* user_data[2] = {data, (void*)(uintptr_t)tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_kahan_state_fn, user_data, &result);

        free(data);
    }
}

/* Benchmark with catastrophic cancellation scenario */
static void run_cancellation_benchmark(void) {
    printf("\nCatastrophic Cancellation Scenario\n");
    printf("------------------------------------------------------------\n");

    const size_t n = 100000;
    double* data = (double*)malloc(n * sizeof(double));
    if (!data) return;

    /* Create alternating large and small values */
    for (size_t i = 0; i < n; i++) {
        data[i] = (i % 2 == 0) ? 1e10 : 1.0;
    }

    void* user_data[2] = {data, (void*)(uintptr_t)n};

    /* Benchmark Kahan sum */
    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = "KahanSum/Cancellation/100K";
    config.data_size = n * sizeof(double);
    config.min_time_ms = 100.0;
    config.quiet = 0;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_kahan_sum_fn, user_data, &result);

    /* Benchmark naive sum */
    config.name = "NaiveSum/Cancellation/100K";
    fc_bench_run(&config, bench_naive_sum_fn, user_data, &result);

    free(data);
}

/* Entry point */
void bench_kahan_sum_run(void) {
    printf("\n");
    printf("Kahan Compensated Summation Benchmarks\n");
    printf("\n");

    run_kahan_benchmarks();
    run_naive_benchmarks();
    run_state_benchmarks();
    run_cancellation_benchmark();

    printf("\n");
    printf("\n");
    printf("Performance Notes\n");
    printf("\n");
    printf("Kahan summation provides higher numerical accuracy than\n");
    printf("naive summation with ~4x performance overhead (scalar).\n");

    printf("\n");
}
