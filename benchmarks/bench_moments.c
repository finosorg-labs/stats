/**
 * @file bench_moments.c
 * @brief Benchmarks for higher-order moments
 */

#include "bench_framework.h"
#include "../include/moments.h"
#include <stdlib.h>
#include <string.h>

/* Benchmark data sizes */
#define SMALL_SIZE   100
#define MEDIUM_SIZE  1000
#define LARGE_SIZE   10000

/*
 * Helper functions
 */

static void generate_random_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)rand() / RAND_MAX * 100.0;
    }
}

/*
 * Benchmark functions - Online moments
 */

static void bench_moments_update_single_fn(void* user_data) {
    fc_moments_state_t* state = (fc_moments_state_t*)user_data;
    fc_stats_moments_update(state, 42.0);
}

static void bench_moments_update_batch_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n = (size_t)(uintptr_t)data_ptr[1];

    fc_moments_state_t state;
    fc_stats_moments_init(&state);
    fc_stats_moments_update_batch(&state, data, n);
}

/*
 * Benchmark functions - Batch skewness
 */

typedef struct {
    double* data;
    size_t n_groups;
    size_t group_size;
    double* out;
} batch_moments_data_t;

static void bench_skewness_batch_fn(void* user_data) {
    batch_moments_data_t* bd = (batch_moments_data_t*)user_data;
    fc_stats_skewness_f64(bd->out, bd->data, bd->n_groups, bd->group_size);
}

static void bench_kurtosis_batch_fn(void* user_data) {
    batch_moments_data_t* bd = (batch_moments_data_t*)user_data;
    fc_stats_kurtosis_f64(bd->out, bd->data, bd->n_groups, bd->group_size);
}

/*
 * Benchmark runners
 */

static void run_online_update_benchmarks(void) {
    printf("\nOnline Moments Update Benchmarks\n");
    printf("------------------------------------------------------------\n");

    /* Single update benchmark */
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = "MomentsUpdate/Single";
    config.min_time_ms = 100.0;
    config.quiet = 0;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_moments_update_single_fn, &state, &result);

    /* Batch update benchmarks */
    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"MomentsUpdate/Batch/100", SMALL_SIZE},
        {"MomentsUpdate/Batch/1K", MEDIUM_SIZE},
        {"MomentsUpdate/Batch/10K", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        if (!data) continue;

        generate_random_data(data, tests[i].size);

        void* user_data[2] = {data, (void*)(uintptr_t)tests[i].size};

        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_run(&config, bench_moments_update_batch_fn, user_data, &result);

        free(data);
    }
}

static void run_batch_skewness_benchmarks(void) {
    printf("\nBatch Skewness Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n_groups;
        size_t group_size;
    } tests[] = {
        {"Skewness/10x100", 10, 100},
        {"Skewness/100x100", 100, 100},
        {"Skewness/1000x100", 1000, 100},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t n_groups = tests[i].n_groups;
        size_t group_size = tests[i].group_size;
        size_t total_size = n_groups * group_size;

        double* data = (double*)malloc(total_size * sizeof(double));
        double* out = (double*)malloc(n_groups * sizeof(double));
        if (!data || !out) {
            free(data);
            free(out);
            continue;
        }

        generate_random_data(data, total_size);

        batch_moments_data_t bd = {
            .data = data,
            .n_groups = n_groups,
            .group_size = group_size,
            .out = out
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = total_size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_skewness_batch_fn, &bd, &result);

        free(data);
        free(out);
    }
}

static void run_batch_kurtosis_benchmarks(void) {
    printf("\nBatch Kurtosis Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n_groups;
        size_t group_size;
    } tests[] = {
        {"Kurtosis/10x100", 10, 100},
        {"Kurtosis/100x100", 100, 100},
        {"Kurtosis/1000x100", 1000, 100},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t n_groups = tests[i].n_groups;
        size_t group_size = tests[i].group_size;
        size_t total_size = n_groups * group_size;

        double* data = (double*)malloc(total_size * sizeof(double));
        double* out = (double*)malloc(n_groups * sizeof(double));
        if (!data || !out) {
            free(data);
            free(out);
            continue;
        }

        generate_random_data(data, total_size);

        batch_moments_data_t bd = {
            .data = data,
            .n_groups = n_groups,
            .group_size = group_size,
            .out = out
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = total_size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_kurtosis_batch_fn, &bd, &result);

        free(data);
        free(out);
    }
}

/*
 * Entry point
 */

void bench_moments_run(void) {
    printf("\n");
    printf("Higher-Order Moments Benchmarks\n");
    printf("============================================================\n");

    run_online_update_benchmarks();
    run_batch_skewness_benchmarks();
    run_batch_kurtosis_benchmarks();

    printf("\n");
    printf("============================================================\n");
}
