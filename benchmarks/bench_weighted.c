/**
 * @file bench_weighted.c
 * @brief Benchmarks for weighted statistical computations
 */

#include "bench_framework.h"
#include "../include/weighted.h"
#include <stdint.h>
#include <stdlib.h>

#define SMALL_SIZE  100
#define MEDIUM_SIZE 10000
#define LARGE_SIZE  1000000

static void generate_weighted_data(double* data, double* weights, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)(i % 1000) - 500.0;
        weights[i] = (double)(i % 10) + 1.0;
    }
}

typedef struct {
    double* data;
    double* weights;
    size_t n;
} weighted_bench_data_t;

typedef struct {
    double* data;
    double* weights;
    double* means;
    double* variances;
    size_t n_groups;
    size_t group_size;
} weighted_batch_bench_data_t;

static void bench_weighted_mean_fn(void* user_data) {
    weighted_bench_data_t* bd = (weighted_bench_data_t*)user_data;
    double mean;
    fc_stats_weighted_mean_f64(bd->data, bd->weights, bd->n, &mean);
}

static void bench_weighted_variance_fn(void* user_data) {
    weighted_bench_data_t* bd = (weighted_bench_data_t*)user_data;
    double variance;
    fc_stats_weighted_variance_f64(bd->data, bd->weights, bd->n, &variance);
}

static void bench_weighted_mean_variance_fn(void* user_data) {
    weighted_bench_data_t* bd = (weighted_bench_data_t*)user_data;
    double mean;
    double variance;
    fc_stats_weighted_mean_variance_f64(bd->data, bd->weights, bd->n, &mean, &variance);
}

static void bench_weighted_mean_variance_batch_fn(void* user_data) {
    weighted_batch_bench_data_t* bd = (weighted_batch_bench_data_t*)user_data;
    fc_stats_weighted_mean_variance_batch_f64(
        bd->means,
        bd->variances,
        bd->data,
        bd->weights,
        bd->n_groups,
        bd->group_size
    );
}

static void run_single_benchmarks(const char* title, const char* prefix, fc_bench_fn fn) {
    printf("\n%s\n", title);
    printf("------------------------------------------------------------\n");

    struct {
        const char* suffix;
        size_t size;
    } tests[] = {
        {"Small/100", SMALL_SIZE},
        {"Medium/10K", MEDIUM_SIZE},
        {"Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        double* weights = (double*)malloc(tests[i].size * sizeof(double));
        if (data == NULL || weights == NULL) {
            free(data);
            free(weights);
            continue;
        }

        generate_weighted_data(data, weights, tests[i].size);

        weighted_bench_data_t bench_data = {data, weights, tests[i].size};
        char name[128];
        snprintf(name, sizeof(name), "%s/%s", prefix, tests[i].suffix);

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = name;
        config.data_size = tests[i].size * sizeof(double) * 2;
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, fn, &bench_data, &result);

        free(data);
        free(weights);
    }
}

static void run_weighted_batch_benchmarks(void) {
    printf("\nWeighted Batch Mean+Variance Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n_groups;
        size_t group_size;
    } tests[] = {
        {"WeightedBatch/10x100", 10, 100},
        {"WeightedBatch/100x100", 100, 100},
        {"WeightedBatch/250x5000", 250, 5000},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        size_t n = tests[i].n_groups * tests[i].group_size;
        double* data = (double*)malloc(n * sizeof(double));
        double* weights = (double*)malloc(n * sizeof(double));
        double* means = (double*)malloc(tests[i].n_groups * sizeof(double));
        double* variances = (double*)malloc(tests[i].n_groups * sizeof(double));
        if (data == NULL || weights == NULL || means == NULL || variances == NULL) {
            free(data);
            free(weights);
            free(means);
            free(variances);
            continue;
        }

        generate_weighted_data(data, weights, n);

        weighted_batch_bench_data_t bench_data = {
            data,
            weights,
            means,
            variances,
            tests[i].n_groups,
            tests[i].group_size,
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = n * sizeof(double) * 2;
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_weighted_mean_variance_batch_fn, &bench_data, &result);

        free(data);
        free(weights);
        free(means);
        free(variances);
    }
}

void bench_weighted_run(void) {
    run_single_benchmarks("Weighted Mean Benchmarks", "WeightedMean", bench_weighted_mean_fn);
    run_single_benchmarks("Weighted Variance Benchmarks", "WeightedVariance", bench_weighted_variance_fn);
    run_single_benchmarks("Weighted Mean+Variance Benchmarks", "WeightedMeanVar", bench_weighted_mean_variance_fn);
    run_weighted_batch_benchmarks();
}
