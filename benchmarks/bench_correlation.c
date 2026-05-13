/**
 * @file bench_correlation.c
 * @brief Benchmark tests for correlation coefficient computation
 */

#include "bench_framework.h"
#include "../include/correlation.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Benchmark: Pairwise correlation */
static void bench_correlation_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* x = data_ptr[0];
    double* y = data_ptr[1];
    size_t n = (size_t)(uintptr_t)data_ptr[2];
    double corr;
    fc_stats_pearson_correlation_f64(x, y, n, &corr);
}

/* Benchmark: Correlation matrix */
static void bench_correlation_matrix_fn(void* user_data) {
    double** data_ptr = (double**)user_data;
    double* data = data_ptr[0];
    size_t n_samples = (size_t)(uintptr_t)data_ptr[1];
    size_t n_vars = (size_t)(uintptr_t)data_ptr[2];
    double* corr_matrix = data_ptr[3];
    fc_stats_pearson_correlation_matrix_f64(data, n_samples, n_vars, corr_matrix);
}

/* Benchmark runners */
static void run_pairwise_correlation_benchmarks(void) {
    printf("\nPairwise Correlation Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"Correlation/n=250", 250},
        {"Correlation/n=1000", 1000},
        {"Correlation/n=10000", 10000},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* x = (double*)malloc(tests[i].size * sizeof(double));
        double* y = (double*)malloc(tests[i].size * sizeof(double));
        if (!x || !y) {
            free(x);
            free(y);
            continue;
        }

        /* Initialize with random-like data */
        for (size_t j = 0; j < tests[i].size; j++) {
            x[j] = (double)j * 0.5;
            y[j] = (double)j * 0.3 + 10.0;
        }

        void* user_data[3] = {x, y, (void*)(uintptr_t)tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double) * 2;
        config.min_iterations = 100;
        config.min_time_ms = 100.0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_correlation_fn, user_data, &result);

        free(x);
        free(y);
    }
}

static void run_correlation_matrix_benchmarks(void) {
    printf("\nCorrelation Matrix Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n_vars;
        size_t n_samples;
    } tests[] = {
        {"CorrelationMatrix/10vars×250samples", 10, 250},
        {"CorrelationMatrix/50vars×250samples", 50, 250},
        {"CorrelationMatrix/100vars×250samples", 100, 250},
        {"CorrelationMatrix/500vars×250samples", 500, 250},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t n_vars = tests[i].n_vars;
        size_t n_samples = tests[i].n_samples;

        double* data = (double*)malloc(n_samples * n_vars * sizeof(double));
        double* corr_matrix = (double*)malloc(n_vars * n_vars * sizeof(double));
        if (!data || !corr_matrix) {
            free(data);
            free(corr_matrix);
            continue;
        }

        /* Initialize data */
        for (size_t j = 0; j < n_samples; j++) {
            for (size_t k = 0; k < n_vars; k++) {
                data[j * n_vars + k] = (double)(j + k) * 0.5;
            }
        }

        void* user_data[4] = {
            data,
            (void*)(uintptr_t)n_samples,
            (void*)(uintptr_t)n_vars,
            corr_matrix
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = n_samples * n_vars * sizeof(double);
        config.min_iterations = (n_vars >= 500) ? 10 : 50;
        config.min_time_ms = (n_vars >= 500) ? 500.0 : 200.0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_correlation_matrix_fn, user_data, &result);

        free(data);
        free(corr_matrix);
    }

    printf("\nPerformance target: 500 vars × 250 samples < 50ms (vs Go > 250ms)\n");
}

/* Public entry point */
void bench_correlation_run(void) {
    printf("\n=== Correlation Coefficient Benchmarks ===\n");
    run_pairwise_correlation_benchmarks();
    run_correlation_matrix_benchmarks();
}
