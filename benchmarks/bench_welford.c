/**
 * @file bench_welford.c
 * @brief Benchmarks for Welford's online statistics
 */

#include "../include/welford.h"
#include "bench_framework.h"
#include "simd_detect.h"
#include <stdio.h>
#include <stdlib.h>

#define SMALL_SIZE   100
#define MEDIUM_SIZE  10000
#define LARGE_SIZE   1000000

/* Helper to generate sequential data */
static void generate_sequential_data(double* data, size_t n) {
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)i;
    }
}

/* Benchmark functions for single update */
static void bench_welford_update_fn(void* user_data) {
    void** params = (void**)user_data;
    double* data = (double*)params[0];
    size_t n = (size_t)(uintptr_t)params[1];

    fc_welford_state_t state;
    fc_welford_init(&state);

    for (size_t i = 0; i < n; i++) {
        fc_welford_update(&state, data[i]);
    }

    volatile double result = state.mean;
    (void)result;
}

/* Benchmark functions for batch update */
static void bench_welford_batch_fn(void* user_data) {
    void** params = (void**)user_data;
    double* data = (double*)params[0];
    size_t n = (size_t)(uintptr_t)params[1];

    fc_welford_state_t state;
    fc_welford_init(&state);
    fc_welford_update_batch(&state, data, n);

    volatile double result = state.mean;
    (void)result;
}

/* Benchmark functions for merge */
static void bench_welford_merge_fn(void* user_data) {
    void** params = (void**)user_data;
    fc_welford_state_t* states = (fc_welford_state_t*)params[0];
    size_t n_states = (size_t)(uintptr_t)params[1];

    fc_welford_state_t merged;
    fc_welford_init(&merged);

    for (size_t i = 0; i < n_states; i++) {
        fc_welford_merge(&merged, &states[i]);
    }

    volatile double result = merged.mean;
    (void)result;
}

/* Benchmark functions for multi-stream */
static void bench_welford_streams_fn(void* user_data) {
    void** params = (void**)user_data;
    fc_welford_state_t* states = (fc_welford_state_t*)params[0];
    double* values = (double*)params[1];
    size_t n_streams = (size_t)(uintptr_t)params[2];

    fc_welford_update_streams(states, values, n_streams);

    volatile double result = states[0].mean;
    (void)result;
}

/* Run single update benchmarks */
static void run_update_benchmarks(void) {
    printf("\nWelford Single Update Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"WelfordUpdate/Small/100", SMALL_SIZE},
        {"WelfordUpdate/Medium/10K", MEDIUM_SIZE},
        {"WelfordUpdate/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        if (!data) continue;

        generate_sequential_data(data, tests[i].size);

        void* params[2] = {data, (void*)(uintptr_t)tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_welford_update_fn, params, &result);

        free(data);
    }
}

/* Run batch update benchmarks */
static void run_batch_benchmarks(void) {
    printf("\nWelford Batch Update Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t size;
    } tests[] = {
        {"WelfordBatch/Small/100", SMALL_SIZE},
        {"WelfordBatch/Medium/10K", MEDIUM_SIZE},
        {"WelfordBatch/Large/1M", LARGE_SIZE},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        double* data = (double*)malloc(tests[i].size * sizeof(double));
        if (!data) continue;

        generate_sequential_data(data, tests[i].size);

        void* params[2] = {data, (void*)(uintptr_t)tests[i].size};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].size * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_welford_batch_fn, params, &result);

        free(data);
    }
}

/* Run merge benchmarks */
static void run_merge_benchmarks(void) {
    printf("\nWelford Merge Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n_states;
        size_t values_per_state;
    } tests[] = {
        {"WelfordMerge/Small/10", 10, 100},
        {"WelfordMerge/Medium/1K", 1000, 100},
        {"WelfordMerge/Large/10K", 10000, 100},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        fc_welford_state_t* states = (fc_welford_state_t*)malloc(
            tests[i].n_states * sizeof(fc_welford_state_t));
        if (!states) continue;

        // Initialize states with data
        for (size_t j = 0; j < tests[i].n_states; j++) {
            fc_welford_init(&states[j]);
            for (size_t k = 0; k < tests[i].values_per_state; k++) {
                fc_welford_update(&states[j], (double)(j * tests[i].values_per_state + k));
            }
        }

        void* params[2] = {states, (void*)(uintptr_t)tests[i].n_states};

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].n_states * sizeof(fc_welford_state_t);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_welford_merge_fn, params, &result);

        free(states);
    }
}

/* Run multi-stream SIMD benchmarks */
static void run_streams_benchmarks(void) {
    printf("\nWelford Multi-Stream SIMD Benchmarks\n");
    printf("------------------------------------------------------------\n");

    struct {
        const char* name;
        size_t n_streams;
    } tests[] = {
        {"WelfordStreams/SSE/2x10K", 2},
        {"WelfordStreams/AVX2/4x10K", 4},
        {"WelfordStreams/AVX512/8x10K", 8},
        {"WelfordStreams/Market/1000x1K", 1000},
    };

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        fc_welford_state_t* states = (fc_welford_state_t*)malloc(
            tests[i].n_streams * sizeof(fc_welford_state_t));
        double* values = (double*)malloc(tests[i].n_streams * sizeof(double));

        if (!states || !values) {
            free(states);
            free(values);
            continue;
        }

        // Initialize states and values
        for (size_t j = 0; j < tests[i].n_streams; j++) {
            fc_welford_init(&states[j]);
            values[j] = (double)j;
        }

        void* params[3] = {
            states,
            values,
            (void*)(uintptr_t)tests[i].n_streams
        };

        fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
        config.name = tests[i].name;
        config.data_size = tests[i].n_streams * sizeof(double);
        config.min_time_ms = 100.0;
        config.quiet = 0;

        fc_bench_result_t result;
        fc_bench_run(&config, bench_welford_streams_fn, params, &result);

        free(states);
        free(values);
    }
}

void bench_welford_run(void) {
    printf("\n");
    printf("Welford Online Statistics Benchmarks\n");
    printf("\n");

    run_update_benchmarks();
    run_batch_benchmarks();
    run_merge_benchmarks();
    run_streams_benchmarks();

    printf("\n");
    printf("\n");
    printf("Performance Notes\n");
    printf("\n");
    printf("Welford's algorithm provides numerically stable online\n");
    printf("computation of mean and variance with minimal memory.\n");
    printf("Batch updates use divide-and-conquer for cache locality.\n");
    printf("Multi-stream SIMD: processes N independent streams in parallel.\n");

    printf("  - Ideal for real-time market statistics (1000+ stocks)\n");
    printf("\n");
}
