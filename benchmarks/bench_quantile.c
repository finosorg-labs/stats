/**
 * @file bench_quantile.c
 * @brief Benchmarks for quantile computation
 */

#include "bench_framework.h"
#include "../include/quantile.h"
#include <stdlib.h>
#include <string.h>

/* Benchmark data sizes */
#define SMALL_SIZE   100
#define MEDIUM_SIZE  10000
#define LARGE_SIZE   1000000

/*
 * Helper functions
 */

static void generate_random_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double) rand() / RAND_MAX * 1000.0;
    }
}

/*
 * Benchmark functions
 */

static void bench_median_fn(void* user_data) {
    double** data_ptr = (double**) user_data;
    double* data      = data_ptr[0];
    size_t n          = (size_t) (uintptr_t) data_ptr[1];
    double result;
    fc_stats_median_f64(data, n, &result);
}

static void bench_quantile_fn(void* user_data) {
    double** data_ptr = (double**) user_data;
    double* data      = data_ptr[0];
    size_t n          = (size_t) (uintptr_t) data_ptr[1];
    double result;
    fc_stats_quantile_f64(data, n, 0.75, &result);
}

static void bench_quartiles_fn(void* user_data) {
    double** data_ptr = (double**) user_data;
    double* data      = data_ptr[0];
    size_t n          = (size_t) (uintptr_t) data_ptr[1];
    double q1, q2, q3;
    fc_stats_quartiles_f64(data, n, &q1, &q2, &q3);
}

/*
 * Benchmark runners
 */

static void run_median_benchmarks(void) {
    printf("\nMedian Computation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Median/Small/100", SMALL_SIZE},
        {"Median/Medium/10K", MEDIUM_SIZE},
        {"Median/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*) malloc(tests[i].size * sizeof(double));
        if (!data)
            continue;

        generate_random_data(data, tests[i].size);

        void* user_data[2] = {data, (void*) (uintptr_t) tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);
        config.warmup_ms         = 100;
        config.min_iterations    = 10;
        config.min_time_ms       = 1000;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_median_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
    }
}

static void run_quantile_benchmarks(void) {
    printf("\nQuantile (Q3) Computation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Quantile/Small/100", SMALL_SIZE},
        {"Quantile/Medium/10K", MEDIUM_SIZE},
        {"Quantile/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*) malloc(tests[i].size * sizeof(double));
        if (!data)
            continue;

        generate_random_data(data, tests[i].size);

        void* user_data[2] = {data, (void*) (uintptr_t) tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);
        config.warmup_ms         = 100;
        config.min_iterations    = 10;
        config.min_time_ms       = 1000;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_quantile_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
    }
}

static void run_quartiles_benchmarks(void) {
    printf("\nQuartiles Computation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Quartiles/Small/100", SMALL_SIZE},
        {"Quartiles/Medium/10K", MEDIUM_SIZE},
        {"Quartiles/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*) malloc(tests[i].size * sizeof(double));
        if (!data)
            continue;

        generate_random_data(data, tests[i].size);

        void* user_data[2] = {data, (void*) (uintptr_t) tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);
        config.warmup_ms         = 100;
        config.min_iterations    = 10;
        config.min_time_ms       = 1000;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_quartiles_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
    }
}

/*
 * Main benchmark entry point
 */

void bench_quantile_run_all(void) {
    printf("\n");
    printf("============================================================\n");
    printf(" Quantile Computation Benchmarks\n");
    printf("============================================================\n");

    run_median_benchmarks();
    run_quantile_benchmarks();
    run_quartiles_benchmarks();

    printf("\n");
}
