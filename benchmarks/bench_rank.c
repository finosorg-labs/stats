/**
 * @file bench_rank.c
 * @brief Benchmarks for rank computation
 */

#include "bench_framework.h"
#include "../include/rank.h"
#include <stdlib.h>
#include <string.h>

/* Benchmark data sizes */
#define SMALL_SIZE   100
#define MEDIUM_SIZE  1000
#define LARGE_SIZE   10000
#define XLARGE_SIZE  5000

/*
 * Helper functions
 */

static void generate_random_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)rand() / RAND_MAX * 1000.0;
    }
}

static void generate_data_with_ties(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)(rand() % 100);
    }
}

/*
 * Benchmark functions
 */

static void bench_rank_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    double* ranks = data_ptr[1];
    size_t n = (size_t)(uintptr_t)data_ptr[2];
    fc_stats_rank_f64(data, n, ranks, FC_RANK_AVERAGE);
}

static void bench_rank_normalized_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    double* ranks = data_ptr[1];
    size_t n = (size_t)(uintptr_t)data_ptr[2];
    fc_stats_rank_normalized_f64(data, n, ranks, FC_RANK_AVERAGE);
}

static void bench_rank_batch_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    double* ranks = data_ptr[1];
    size_t n = (size_t)(uintptr_t)data_ptr[2];
    size_t num_arrays = (size_t)(uintptr_t)data_ptr[3];
    fc_stats_rank_batch_f64(data, n, num_arrays, ranks, FC_RANK_AVERAGE);
}

/*
 * Benchmark runners
 */

static void run_rank_basic_benchmarks(void) {
    printf("\nRank Computation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Rank/Small/100", SMALL_SIZE},
        {"Rank/Medium/1K", MEDIUM_SIZE},
        {"Rank/Large/10K", LARGE_SIZE},
        {"Rank/CrossSection/5K", XLARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        double* ranks = (double*)malloc(tests[i].size * sizeof(double));
        if (!data || !ranks) {
            free(data);
            free(ranks);
            continue;
        }

        generate_random_data(data, tests[i].size);

        void* user_data[3] = {data, ranks, (void*)(uintptr_t)tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double);
        config.warmup_ms = 100;
        config.min_iterations = 10;
        config.min_time_ms = 1000;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_rank_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
        free(ranks);
    }
}

static void run_rank_methods_benchmarks(void) {
    printf("\nRank Methods Comparison (5K elements)\n");
    printf("------------------------------------------------------------\n");

    const size_t n = XLARGE_SIZE;
    double* data = (double*)malloc(n * sizeof(double));
    double* ranks = (double*)malloc(n * sizeof(double));
    if (!data || !ranks) {
        free(data);
        free(ranks);
        return;
    }

    generate_random_data(data, n);

    struct {
        const char* name;
        fc_rank_method_t method;
    } methods[] = {
        {"Rank/Method/Average", FC_RANK_AVERAGE},
        {"Rank/Method/Min", FC_RANK_MIN},
        {"Rank/Method/Max", FC_RANK_MAX},
        {"Rank/Method/First", FC_RANK_FIRST},
        {"Rank/Method/Dense", FC_RANK_DENSE},
    };

    for (size_t i = 0; i < sizeof(methods) / sizeof(methods[0]); i++) {
        void* user_data[3] = {data, ranks, (void*)(uintptr_t)n};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = methods[i].name;
        config.data_size = n * sizeof(double);
        config.warmup_ms = 100;
        config.min_iterations = 10;
        config.min_time_ms = 1000;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_rank_fn, user_data, &result);
        fc_bench_result_print(&result);
    }

    free(data);
    free(ranks);
}

static void run_rank_normalized_benchmarks(void) {
    printf("\nNormalized Rank Benchmarks\n");
    printf("------------------------------------------------------------\n");

    const size_t n = XLARGE_SIZE;
    double* data = (double*)malloc(n * sizeof(double));
    double* ranks = (double*)malloc(n * sizeof(double));
    if (!data || !ranks) {
        free(data);
        free(ranks);
        return;
    }

    generate_random_data(data, n);

    void* user_data[3] = {data, ranks, (void*)(uintptr_t)n};

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = "Rank/Normalized/5K";
    config.data_size = n * sizeof(double);
    config.warmup_ms = 100;
    config.min_iterations = 10;
    config.min_time_ms = 1000;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_rank_normalized_fn, user_data, &result);
    fc_bench_result_print(&result);

    free(data);
    free(ranks);
}

static void run_rank_batch_benchmarks(void) {
    printf("\nBatch Rank Benchmarks (Factor Analysis)\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n;
        size_t num_arrays;
    } tests[] = {
        {"Rank/Batch/100factors_5K", XLARGE_SIZE, 100},
        {"Rank/Batch/500factors_5K", XLARGE_SIZE, 500},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t total_size = tests[i].n * tests[i].num_arrays;
        double* data = (double*)malloc(total_size * sizeof(double));
        double* ranks = (double*)malloc(total_size * sizeof(double));
        if (!data || !ranks) {
            free(data);
            free(ranks);
            continue;
        }

        generate_random_data(data, total_size);

        void* user_data[4] = {
            data,
            ranks,
            (void*)(uintptr_t)tests[i].n,
            (void*)(uintptr_t)tests[i].num_arrays
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = total_size * sizeof(double);
        config.warmup_ms = 100;
        config.min_iterations = 5;
        config.min_time_ms = 1000;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_rank_batch_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
        free(ranks);
    }
}

static void run_rank_with_ties_benchmarks(void) {
    printf("\nRank with Ties Benchmarks\n");
    printf("------------------------------------------------------------\n");

    const size_t n = XLARGE_SIZE;
    double* data = (double*)malloc(n * sizeof(double));
    double* ranks = (double*)malloc(n * sizeof(double));
    if (!data || !ranks) {
        free(data);
        free(ranks);
        return;
    }

    generate_data_with_ties(data, n);

    void* user_data[3] = {data, ranks, (void*)(uintptr_t)n};

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    config.name = "Rank/WithTies/5K";
    config.data_size = n * sizeof(double);
    config.warmup_ms = 100;
    config.min_iterations = 10;
    config.min_time_ms = 1000;

    fc_bench_result_t result;
    fc_bench_run(&config, bench_rank_fn, user_data, &result);
    fc_bench_result_print(&result);

    free(data);
    free(ranks);
}

/*
 * Main benchmark entry point
 */

void bench_rank_run(void) {
    printf("\n");
    printf("============================================================\n");
    printf(" Rank Computation Benchmarks\n");
    printf("============================================================\n");

    run_rank_basic_benchmarks();
    run_rank_methods_benchmarks();
    run_rank_normalized_benchmarks();
    run_rank_batch_benchmarks();
    run_rank_with_ties_benchmarks();

    printf("\n");
}
