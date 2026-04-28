/**
 * @file bench_mean_var.c
 * @brief Benchmarks for mean and variance computation
 */

#include "bench_framework.h"
#include "../include/mean_var.h"
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
        data[i] = (double)rand() / RAND_MAX * 1000.0 - 500.0;
    }
}

/*
 * Benchmark functions
*/

static void bench_mean_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n = (size_t)(uintptr_t)data_ptr[1];
    double mean;
    fc_stats_mean_f64(data, n, &mean);
}

static void bench_variance_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n = (size_t)(uintptr_t)data_ptr[1];
    double variance;
    fc_stats_variance_f64(data, n, &variance, 1);
}

static void bench_mean_variance_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n = (size_t)(uintptr_t)data_ptr[1];
    double mean, variance;
    fc_stats_mean_variance_f64(data, n, &mean, &variance, 1);
}

/*
 * Benchmark runners
*/

static void run_mean_benchmarks(void) {
    printf("\nMean Computation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Mean/Small/100", SMALL_SIZE},
        {"Mean/Medium/10K", MEDIUM_SIZE},
        {"Mean/Large/1M", LARGE_SIZE},
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
        fc_bench_run(&config, bench_mean_fn, user_data, &result);

        free(data);
    }
}

static void run_variance_benchmarks(void) {
    printf("\nVariance Computation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Variance/Small/100", SMALL_SIZE},
        {"Variance/Medium/10K", MEDIUM_SIZE},
        {"Variance/Large/1M", LARGE_SIZE},
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
        fc_bench_run(&config, bench_variance_fn, user_data, &result);

        free(data);
    }
}

static void run_mean_variance_benchmarks(void) {
    printf("\nMean+Variance Combined Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"MeanVar/Small/100", SMALL_SIZE},
        {"MeanVar/Medium/10K", MEDIUM_SIZE},
        {"MeanVar/Large/1M", LARGE_SIZE},
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
        fc_bench_run(&config, bench_mean_variance_fn, user_data, &result);

        free(data);
    }
}

/*
 * Batch benchmark functions
*/

typedef struct {
    const double** data;
    size_t n;
    size_t num_arrays;
    double* means;
    double* variances;
} batch_bench_data_t;

static void bench_mean_variance_batch_fn(void* user_data) {
    batch_bench_data_t* bd = (batch_bench_data_t*)user_data;
    fc_stats_mean_variance_batch_f64(bd->data, bd->n, bd->num_arrays,
                                     bd->means, bd->variances, 1);
}

static void run_batch_benchmarks(void) {
    printf("\nBatch Mean+Variance Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n;
        size_t num_arrays;
    } tests[] = {
        {"Batch/100x100", 100, 100},
        {"Batch/250x1000", 250, 1000},
        {"Batch/250x5000 [TARGET]", 250, 5000},  /* Planning target */
        {"Batch/1000x1000", 1000, 1000},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t n = tests[i].n;
        size_t num_arrays = tests[i].num_arrays;

        /* Allocate data arrays */
        double** data = (double**)malloc(num_arrays * sizeof(double*));
        if (!data) continue;

        for (size_t j = 0; j < num_arrays; j++) {
            data[j] = (double*)malloc(n * sizeof(double));
            if (!data[j]) {
                for (size_t k = 0; k < j; k++) free(data[k]);
                free(data);
                goto next_test;
            }
            generate_random_data(data[j], n);
        }

        /* Allocate output arrays */
        double* means = (double*)malloc(num_arrays * sizeof(double));
        double* variances = (double*)malloc(num_arrays * sizeof(double));
        if (!means || !variances) {
            for (size_t j = 0; j < num_arrays; j++) free(data[j]);
            free(data);
            free(means);
            free(variances);
            continue;
        }

        /* Setup benchmark data */
        batch_bench_data_t bd = {
            .data = (const double**)data,
            .n = n,
            .num_arrays = num_arrays,
            .means = means,
            .variances = variances
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = n * num_arrays * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_mean_variance_batch_fn, &bd, &result);

        /* Check if target is met (5000x250 < 0.3ms) */
        if (num_arrays == 5000 && n == 250) {
            printf("  Target check: %.3f ms %s (target: < 0.3 ms)\n",
                   result.mean_ns / 1e6,
                   (result.mean_ns / 1e6 < 0.3) ? "[PASS]" : "[FAIL]");
        }

        /* Cleanup */
        for (size_t j = 0; j < num_arrays; j++) free(data[j]);
        free(data);
        free(means);
        free(variances);

next_test:
        continue;
    }
}

/*
 * Entry point
*/

void bench_mean_var_run(void) {
    printf("\n");
    printf("Statistics Mean/Variance Benchmarks\n");
    printf("============================================================\n");

    run_mean_benchmarks();
    run_variance_benchmarks();
    run_mean_variance_benchmarks();
    run_batch_benchmarks();

    printf("\n");
    printf("============================================================\n");
    printf("Performance Target Summary\n");
    printf("============================================================\n");
    printf("Target: 5000 groups × 250 elements < 0.3 ms\n");
    printf("See 'Batch/250x5000 [TARGET]' result above\n");
    printf("============================================================\n");
}
