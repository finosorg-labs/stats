/**
 * @file bench_ema.c
 * @brief Benchmarks for EMA computation
 */

#include "bench_framework.h"
#include "../include/ema.h"
#include <stdlib.h>
#include <string.h>

/* Benchmark data sizes */
#define SMALL_SIZE   100
#define MEDIUM_SIZE  10000
#define LARGE_SIZE   100000

/**
 * @brief Generate synthetic time series data
 */
static void generate_time_series(double* data, size_t n, double base, double volatility) {
    data[0] = base;
    for (size_t i = 1; i < n; i++) {
        double change = ((double) rand() / RAND_MAX - 0.5) * volatility;
        data[i]       = data[i - 1] * (1.0 + change);
    }
}

/*
 * Benchmark functions
 */

static void bench_ema_fn(void* user_data) {
    double** data_ptr = (double**) user_data;
    double* data      = data_ptr[0];
    size_t n          = (size_t) (uintptr_t) data_ptr[1];
    double* ema       = data_ptr[2];
    double alpha      = 0.2;

    fc_stats_ema_f64(data, n, alpha, ema);
}

static void bench_ema_batch_fn(void* user_data) {
    double** data_ptr = (double**) user_data;
    double* data      = data_ptr[0];
    size_t num_series = (size_t) (uintptr_t) data_ptr[1];
    size_t n          = (size_t) (uintptr_t) data_ptr[2];
    double* ema       = data_ptr[3];
    double alpha      = 0.2;

    fc_stats_ema_batch_f64(data, num_series, n, alpha, ema);
}

/*
 * Benchmark runners
 */

static void run_ema_benchmarks(void) {
    printf("\nEMA Computation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"EMA/Small/100", SMALL_SIZE},
        {"EMA/Medium/10K", MEDIUM_SIZE},
        {"EMA/Large/100K", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*) malloc(tests[i].size * sizeof(double));
        double* ema  = (double*) malloc(tests[i].size * sizeof(double));
        if (!data || !ema) {
            free(data);
            free(ema);
            continue;
        }

        generate_time_series(data, tests[i].size, 100.0, 0.02);

        void* user_data[3] = {data, (void*) (uintptr_t) tests[i].size, ema};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = tests[i].size * sizeof(double);
        config.min_time_ms       = 100.0;
        config.quiet             = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_ema_fn, user_data, &result);

        free(data);
        free(ema);
    }
}

static void run_ema_batch_benchmarks(void) {
    printf("\nBatch EMA Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n;
        size_t num_series;
    } tests[] = {
        {"EMA_Batch/100x100", 100, 100},
        {"EMA_Batch/250x1000", 250, 1000},
        {"EMA_Batch/250x5000", 250, 5000},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t n          = tests[i].n;
        size_t num_series = tests[i].num_series;
        size_t total_size = n * num_series;

        double* data = (double*) malloc(total_size * sizeof(double));
        double* ema  = (double*) malloc(total_size * sizeof(double));
        if (!data || !ema) {
            free(data);
            free(ema);
            continue;
        }

        for (size_t j = 0; j < num_series; j++) {
            generate_time_series(&data[j * n], n, 100.0, 0.02);
        }

        void* user_data[4] = {
            data,
            (void*) (uintptr_t) num_series,
            (void*) (uintptr_t) n,
            ema
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name              = tests[i].name;
        config.data_size         = total_size * sizeof(double);
        config.min_time_ms       = 100.0;
        config.quiet             = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_ema_batch_fn, user_data, &result);

        free(data);
        free(ema);
    }
}

/**
 * @brief Run all EMA benchmarks
 */
void bench_ema_run(void) {
    run_ema_benchmarks();
    run_ema_batch_benchmarks();
}
