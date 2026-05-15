/**
 * @file bench_winsorize.c
 * @brief Benchmark tests for winsorization
 */

#include "bench_framework.h"
#include "../include/winsorize.h"
#include <stdlib.h>
#include <string.h>

#define SMALL_SIZE 1000
#define MEDIUM_SIZE 10000
#define LARGE_SIZE 100000
#define VERY_LARGE_SIZE 1000000

/**
 * @brief Generate random data for benchmarking
 */
static void generate_random_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double) rand() / RAND_MAX * 1000.0 - 500.0;
    }
}

/*
 * Benchmark functions
 */

static void bench_winsorize_fn(void* user_data) {
    double** data_ptr = (double**) user_data;
    double* data      = data_ptr[0];
    double* output    = data_ptr[1];
    size_t n          = (size_t) (uintptr_t) data_ptr[2];
    fc_stats_winsorize_f64(data, n, 0.05, 0.95, output);
}

static void bench_winsorize_inplace_fn(void* user_data) {
    double** data_ptr = (double**) user_data;
    double* data      = data_ptr[0];
    size_t n          = (size_t) (uintptr_t) data_ptr[1];
    fc_stats_winsorize_inplace_f64(data, n, 0.05, 0.95);
}

typedef struct {
    const double** data;
    size_t n;
    size_t num_arrays;
    double** output;
} batch_bench_data_t;

static void bench_winsorize_batch_fn(void* user_data) {
    batch_bench_data_t* bd = (batch_bench_data_t*) user_data;
    fc_stats_winsorize_batch_f64(bd->data, bd->n, bd->num_arrays, 0.05, 0.95, bd->output);
}

/*
 * Benchmark runners
 */

static void run_winsorize_benchmarks(void) {
    printf("\nWinsorize Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Winsorize/Small/1K", SMALL_SIZE},
        {"Winsorize/Medium/10K", MEDIUM_SIZE},
        {"Winsorize/Large/100K", LARGE_SIZE},
        {"Winsorize/VeryLarge/1M", VERY_LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*) malloc(tests[i].size * sizeof(double));
        double* output = (double*) malloc(tests[i].size * sizeof(double));
        if (!data || !output) {
            free(data);
            free(output);
            continue;
        }

        generate_random_data(data, tests[i].size);

        void* user_data[3] = {data, output, (void*) (uintptr_t) tests[i].size};

        fc_bench_config_t config   = FC_BENCH_CONFIG_DEFAULT;
        config.name                = tests[i].name;
        config.data_size           = tests[i].size * sizeof(double);
        config.min_time_ms         = 100.0;
        config.quiet               = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_winsorize_fn, user_data, &result);

        /* Check if target is met for 100K elements */
        if (tests[i].size == LARGE_SIZE) {
            printf("  Target check: %.3f ms %s (target: < 0.5 ms)\n",
                   result.mean_ns / 1e6,
                   (result.mean_ns / 1e6 < 0.5) ? "[PASS]" : "[FAIL]");
        }

        free(data);
        free(output);
    }
}

static void run_winsorize_inplace_benchmarks(void) {
    printf("\nWinsorize In-Place Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"WinsorizeInPlace/Small/1K", SMALL_SIZE},
        {"WinsorizeInPlace/Medium/10K", MEDIUM_SIZE},
        {"WinsorizeInPlace/Large/100K", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*) malloc(tests[i].size * sizeof(double));
        double* backup = (double*) malloc(tests[i].size * sizeof(double));
        if (!data || !backup) {
            free(data);
            free(backup);
            continue;
        }

        generate_random_data(backup, tests[i].size);
        memcpy(data, backup, tests[i].size * sizeof(double));

        void* user_data[2] = {data, (void*) (uintptr_t) tests[i].size};

        fc_bench_config_t config   = FC_BENCH_CONFIG_DEFAULT;
        config.name                = tests[i].name;
        config.data_size           = tests[i].size * sizeof(double);
        config.min_time_ms         = 100.0;
        config.quiet               = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_winsorize_inplace_fn, user_data, &result);

        free(data);
        free(backup);
    }
}

static void run_batch_benchmarks(void) {
    printf("\nWinsorize Batch Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n;
        size_t num_arrays;
    } tests[] = {
        {"Batch/10x1K", 1000, 10},
        {"Batch/10x10K", 10000, 10},
        {"Batch/100x1K", 1000, 100},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t n = tests[i].n;
        size_t num_arrays = tests[i].num_arrays;

        double** data = (double**) malloc(num_arrays * sizeof(double*));
        double** output = (double**) malloc(num_arrays * sizeof(double*));
        if (!data || !output) {
            free(data);
            free(output);
            continue;
        }

        for (size_t j = 0; j < num_arrays; j++) {
            data[j] = (double*) malloc(n * sizeof(double));
            output[j] = (double*) malloc(n * sizeof(double));
            if (!data[j] || !output[j]) {
                for (size_t k = 0; k <= j; k++) {
                    free(data[k]);
                    free(output[k]);
                }
                free(data);
                free(output);
                goto next_test;
            }
            generate_random_data(data[j], n);
        }

        batch_bench_data_t bd = {
            .data = (const double**) data,
            .n = n,
            .num_arrays = num_arrays,
            .output = output
        };

        fc_bench_config_t config   = FC_BENCH_CONFIG_DEFAULT;
        config.name                = tests[i].name;
        config.data_size           = n * num_arrays * sizeof(double);
        config.min_time_ms         = 100.0;
        config.quiet               = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_winsorize_batch_fn, &bd, &result);

        for (size_t j = 0; j < num_arrays; j++) {
            free(data[j]);
            free(output[j]);
        }
        free(data);
        free(output);

    next_test:
        continue;
    }
}

/*
 * Entry point
 */

void bench_winsorize_run(void) {
    printf("\n");
    printf("Winsorization Benchmarks\n");
    printf("============================================================\n");

    run_winsorize_benchmarks();
    run_winsorize_inplace_benchmarks();
    run_batch_benchmarks();

    printf("\n");
    printf("============================================================\n");
    printf("Performance Target Summary\n");
    printf("============================================================\n");
    printf("Target: 100K elements < 0.5 ms\n");
    printf("Target: > 200 million elements/second throughput\n");
    printf("See 'Winsorize/Large/100K' result above\n");
    printf("============================================================\n");
}
