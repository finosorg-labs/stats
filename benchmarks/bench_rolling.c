/**
 * @file bench_rolling.c
 * @brief Benchmarks for rolling window statistics
 */

#include "bench_framework.h"
#include "../include/rolling.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Benchmark data sizes */
#define SMALL_SIZE   1000
#define MEDIUM_SIZE  10000
#define LARGE_SIZE   1000000

/*
 * Helper functions
 */

static void generate_sequential_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double) i;
    }
}

static void generate_random_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double) rand() / RAND_MAX * 1000.0 - 500.0;
    }
}

/*
 * Benchmark functions
 */

static void bench_rolling_mean_fn(void* user_data) {
    void** params = (void**) user_data;
    double* data   = (double*) params[0];
    size_t n       = (size_t)(uintptr_t) params[1];
    size_t window  = (size_t)(uintptr_t) params[2];
    double* output = (double*) params[3];
    fc_stats_rolling_mean_f64(data, n, window, output);
}

static void bench_rolling_variance_fn(void* user_data) {
    void** params = (void**) user_data;
    double* data   = (double*) params[0];
    size_t n       = (size_t)(uintptr_t) params[1];
    size_t window  = (size_t)(uintptr_t) params[2];
    double* output = (double*) params[3];
    fc_stats_rolling_variance_f64(data, n, window, output, 1);
}

static void bench_rolling_sum_fn(void* user_data) {
    void** params = (void**) user_data;
    double* data   = (double*) params[0];
    size_t n       = (size_t)(uintptr_t) params[1];
    size_t window  = (size_t)(uintptr_t) params[2];
    double* output = (double*) params[3];
    fc_stats_rolling_sum_f64(data, n, window, output);
}

static void bench_rolling_min_fn(void* user_data) {
    void** params = (void**) user_data;
    double* data   = (double*) params[0];
    size_t n       = (size_t)(uintptr_t) params[1];
    size_t window  = (size_t)(uintptr_t) params[2];
    double* output = (double*) params[3];
    fc_stats_rolling_min_f64(data, n, window, output);
}

static void bench_rolling_max_fn(void* user_data) {
    void** params = (void**) user_data;
    double* data   = (double*) params[0];
    size_t n       = (size_t)(uintptr_t) params[1];
    size_t window  = (size_t)(uintptr_t) params[2];
    double* output = (double*) params[3];
    fc_stats_rolling_max_f64(data, n, window, output);
}

/*
 * Benchmark runners
 */

static void run_rolling_mean_benchmarks(void) {
    printf("\nRolling Mean Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
        size_t window;
    } tests[] = {
        {"RollingMean/Small/1K/w50", SMALL_SIZE, 50},
        {"RollingMean/Medium/10K/w250", MEDIUM_SIZE, 250},
        {"RollingMean/Large/1M/w250", LARGE_SIZE, 250},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data   = (double*) malloc(tests[i].size * sizeof(double));
        double* output = (double*) malloc(tests[i].size * sizeof(double));
        if (!data || !output) {
            free(data);
            free(output);
            continue;
        }

        generate_sequential_data(data, tests[i].size);

        void* params[4] = {
            data,
            (void*)(uintptr_t) tests[i].size,
            (void*)(uintptr_t) tests[i].window,
            output
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);

        fc_bench_result_t result;
        fc_bench_run(&config, bench_rolling_mean_fn, params, &result);
        fc_bench_result_print(&result);

        free(data);
        free(output);
    }
}

static void run_rolling_variance_benchmarks(void) {
    printf("\nRolling Variance Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
        size_t window;
    } tests[] = {
        {"RollingVariance/Small/1K/w50", SMALL_SIZE, 50},
        {"RollingVariance/Medium/10K/w250", MEDIUM_SIZE, 250},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data   = (double*) malloc(tests[i].size * sizeof(double));
        double* output = (double*) malloc(tests[i].size * sizeof(double));
        if (!data || !output) {
            free(data);
            free(output);
            continue;
        }

        generate_sequential_data(data, tests[i].size);

        void* params[4] = {
            data,
            (void*)(uintptr_t) tests[i].size,
            (void*)(uintptr_t) tests[i].window,
            output
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);

        fc_bench_result_t result;
        fc_bench_run(&config, bench_rolling_variance_fn, params, &result);
        fc_bench_result_print(&result);

        free(data);
        free(output);
    }
}

static void run_rolling_sum_benchmarks(void) {
    printf("\nRolling Sum Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
        size_t window;
    } tests[] = {
        {"RollingSum/Large/1M/w250", LARGE_SIZE, 250},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data   = (double*) malloc(tests[i].size * sizeof(double));
        double* output = (double*) malloc(tests[i].size * sizeof(double));
        if (!data || !output) {
            free(data);
            free(output);
            continue;
        }

        generate_sequential_data(data, tests[i].size);

        void* params[4] = {
            data,
            (void*)(uintptr_t) tests[i].size,
            (void*)(uintptr_t) tests[i].window,
            output
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);

        fc_bench_result_t result;
        fc_bench_run(&config, bench_rolling_sum_fn, params, &result);
        fc_bench_result_print(&result);

        free(data);
        free(output);
    }
}

static void run_rolling_minmax_benchmarks(void) {
    printf("\nRolling Min/Max Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
        size_t window;
        void (*fn)(void*);
    } tests[] = {
        {"RollingMin/Medium/10K/w250", MEDIUM_SIZE, 250, bench_rolling_min_fn},
        {"RollingMax/Medium/10K/w250", MEDIUM_SIZE, 250, bench_rolling_max_fn},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data   = (double*) malloc(tests[i].size * sizeof(double));
        double* output = (double*) malloc(tests[i].size * sizeof(double));
        if (!data || !output) {
            free(data);
            free(output);
            continue;
        }

        generate_random_data(data, tests[i].size);

        void* params[4] = {
            data,
            (void*)(uintptr_t) tests[i].size,
            (void*)(uintptr_t) tests[i].window,
            output
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);

        fc_bench_result_t result;
        fc_bench_run(&config, tests[i].fn, params, &result);
        fc_bench_result_print(&result);

        free(data);
        free(output);
    }
}

/*
 * Public benchmark runner
 */

void bench_rolling_run(void) {
    printf("\n");
    printf("============================================================\n");
    printf(" Rolling Window Statistics Benchmarks\n");
    printf("============================================================\n");

    run_rolling_mean_benchmarks();
    run_rolling_variance_benchmarks();
    run_rolling_sum_benchmarks();
    run_rolling_minmax_benchmarks();
}
