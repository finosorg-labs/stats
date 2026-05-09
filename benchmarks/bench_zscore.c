/**
 * @file bench_zscore.c
 * @brief Benchmarks for Z-score computation
 */

#include "bench_framework.h"
#include "../include/zscore.h"
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

static void bench_zscore_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    double* out = data_ptr[1];
    size_t n = (size_t)(uintptr_t)data_ptr[2];
    fc_stat_zscore_f64(out, data, n, 1);
}

static void bench_zscore_batch_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    double* out = data_ptr[1];
    size_t n_groups = (size_t)(uintptr_t)data_ptr[2];
    size_t group_size = (size_t)(uintptr_t)data_ptr[3];
    fc_stat_zscore_batch_f64(out, data, n_groups, group_size, 1);
}

/*
 * Benchmark suite
*/

void bench_zscore_run(void) {
    printf("\n");
    printf("=================================================================\n");
    printf("Z-Score Benchmarks\n");
    printf("=================================================================\n");

    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    fc_bench_result_t result;

    /* Small size benchmarks */
    {
        double* data = (double*)malloc(SMALL_SIZE * sizeof(double));
        double* out = (double*)malloc(SMALL_SIZE * sizeof(double));
        generate_random_data(data, SMALL_SIZE);

        void* user_data[3];
        user_data[0] = data;
        user_data[1] = out;
        user_data[2] = (void*)(uintptr_t)SMALL_SIZE;

        config.name = "zscore_100";
        config.data_size = SMALL_SIZE * sizeof(double);
        fc_bench_run(&config, bench_zscore_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
        free(out);
    }

    /* Medium size benchmarks */
    {
        double* data = (double*)malloc(MEDIUM_SIZE * sizeof(double));
        double* out = (double*)malloc(MEDIUM_SIZE * sizeof(double));
        generate_random_data(data, MEDIUM_SIZE);

        void* user_data[3];
        user_data[0] = data;
        user_data[1] = out;
        user_data[2] = (void*)(uintptr_t)MEDIUM_SIZE;

        config.name = "zscore_10k";
        config.data_size = MEDIUM_SIZE * sizeof(double);
        fc_bench_run(&config, bench_zscore_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
        free(out);
    }

    /* Large size benchmarks */
    {
        double* data = (double*)malloc(LARGE_SIZE * sizeof(double));
        double* out = (double*)malloc(LARGE_SIZE * sizeof(double));
        generate_random_data(data, LARGE_SIZE);

        void* user_data[3];
        user_data[0] = data;
        user_data[1] = out;
        user_data[2] = (void*)(uintptr_t)LARGE_SIZE;

        config.name = "zscore_1m";
        config.data_size = LARGE_SIZE * sizeof(double);
        fc_bench_run(&config, bench_zscore_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
        free(out);
    }

    /* Batch benchmarks */
    {
        size_t n_groups = 5000;
        size_t group_size = 250;
        size_t total_size = n_groups * group_size;

        double* data = (double*)malloc(total_size * sizeof(double));
        double* out = (double*)malloc(total_size * sizeof(double));
        generate_random_data(data, total_size);

        void* user_data[4];
        user_data[0] = data;
        user_data[1] = out;
        user_data[2] = (void*)(uintptr_t)n_groups;
        user_data[3] = (void*)(uintptr_t)group_size;

        config.name = "zscore_batch_5000x250";
        config.data_size = total_size * sizeof(double);
        fc_bench_run(&config, bench_zscore_batch_fn, user_data, &result);
        fc_bench_result_print(&result);

        free(data);
        free(out);
    }

    printf("\n");
}
