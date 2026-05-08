#include "bench_framework.h"
#include "covariance.h"
#include "mem_aligned.h"
#include <stdlib.h>
#include <math.h>

static void bench_covariance_small(void* user_data) {
    (void)user_data;

    const size_t n_samples = 100;
    const size_t n_vars = 5;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* cov = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1);
    }

    fc_stats_covariance_f64(cov, data, n_samples, n_vars, 1);

    fc_aligned_free(data);
    fc_aligned_free(cov);
}

static void bench_covariance_medium(void* user_data) {
    (void)user_data;

    const size_t n_samples = 1000;
    const size_t n_vars = 20;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* cov = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1);
    }

    fc_stats_covariance_f64(cov, data, n_samples, n_vars, 1);

    fc_aligned_free(data);
    fc_aligned_free(cov);
}

static void bench_covariance_large(void* user_data) {
    (void)user_data;

    const size_t n_samples = 5000;
    const size_t n_vars = 50;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* cov = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1);
    }

    fc_stats_covariance_f64(cov, data, n_samples, n_vars, 1);

    fc_aligned_free(data);
    fc_aligned_free(cov);
}

static void bench_covariance_financial(void* user_data) {
    (void)user_data;

    // Typical financial scenario: 250 trading days, 500 stocks
    const size_t n_samples = 250;
    const size_t n_vars = 500;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* cov = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1) + cos((double)i * 0.05);
    }

    fc_stats_covariance_f64(cov, data, n_samples, n_vars, 1);

    fc_aligned_free(data);
    fc_aligned_free(cov);
}

static void bench_correlation_small(void* user_data) {
    (void)user_data;

    const size_t n_vars = 10;

    double* cov = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);
    double* corr = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    // Initialize with sample covariance matrix
    for (size_t i = 0; i < n_vars; i++) {
        for (size_t j = 0; j < n_vars; j++) {
            if (i == j) {
                cov[i * n_vars + j] = 1.0;
            } else {
                cov[i * n_vars + j] = 0.5 / (1.0 + abs((int)i - (int)j));
            }
        }
    }

    fc_stats_correlation_f64(corr, cov, n_vars);

    fc_aligned_free(cov);
    fc_aligned_free(corr);
}

static void bench_correlation_large(void* user_data) {
    (void)user_data;

    const size_t n_vars = 500;

    double* cov = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);
    double* corr = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    // Initialize with sample covariance matrix
    for (size_t i = 0; i < n_vars; i++) {
        for (size_t j = 0; j < n_vars; j++) {
            if (i == j) {
                cov[i * n_vars + j] = 1.0 + (double)i * 0.01;
            } else {
                cov[i * n_vars + j] = 0.3 / (1.0 + abs((int)i - (int)j));
            }
        }
    }

    fc_stats_correlation_f64(corr, cov, n_vars);

    fc_aligned_free(cov);
    fc_aligned_free(corr);
}

static void bench_covariance_correlation_combined(void* user_data) {
    (void)user_data;

    const size_t n_samples = 250;
    const size_t n_vars = 100;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* cov = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);
    double* corr = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1) + cos((double)i * 0.05);
    }

    fc_stats_covariance_correlation_f64(cov, corr, data, n_samples, n_vars, 1);

    fc_aligned_free(data);
    fc_aligned_free(cov);
    fc_aligned_free(corr);
}

static void bench_spearman_small(void* user_data) {
    (void)user_data;

    const size_t n_samples = 100;
    const size_t n_vars = 5;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* corr = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1);
    }

    fc_stats_spearman_f64(corr, data, n_samples, n_vars);

    fc_aligned_free(data);
    fc_aligned_free(corr);
}

static void bench_spearman_medium(void* user_data) {
    (void)user_data;

    const size_t n_samples = 1000;
    const size_t n_vars = 20;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* corr = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1);
    }

    fc_stats_spearman_f64(corr, data, n_samples, n_vars);

    fc_aligned_free(data);
    fc_aligned_free(corr);
}

static void bench_spearman_large(void* user_data) {
    (void)user_data;

    const size_t n_samples = 5000;
    const size_t n_vars = 50;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* corr = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1);
    }

    fc_stats_spearman_f64(corr, data, n_samples, n_vars);

    fc_aligned_free(data);
    fc_aligned_free(corr);
}

static void bench_spearman_financial(void* user_data) {
    (void)user_data;

    // Typical financial scenario: 250 trading days, 100 stocks
    const size_t n_samples = 250;
    const size_t n_vars = 100;

    double* data = (double*)fc_aligned_alloc(n_samples * n_vars * sizeof(double), 64);
    double* corr = (double*)fc_aligned_alloc(n_vars * n_vars * sizeof(double), 64);

    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double)i * 0.1) + cos((double)i * 0.05);
    }

    fc_stats_spearman_f64(corr, data, n_samples, n_vars);

    fc_aligned_free(data);
    fc_aligned_free(corr);
}

void bench_covariance_run(void) {
    fc_bench_config_t config = FC_BENCH_CONFIG_DEFAULT;
    fc_bench_result_t result;

    printf("=== Covariance and Correlation Benchmarks ===\n\n");

    // Covariance benchmarks
    config.name = "Covariance 100x5";
    config.data_size = 100 * 5 * sizeof(double);
    fc_bench_run(&config, bench_covariance_small, NULL, &result);
    fc_bench_result_print(&result);

    config.name = "Covariance 1000x20";
    config.data_size = 1000 * 20 * sizeof(double);
    fc_bench_run(&config, bench_covariance_medium, NULL, &result);
    fc_bench_result_print(&result);

    config.name = "Covariance 5000x50";
    config.data_size = 5000 * 50 * sizeof(double);
    fc_bench_run(&config, bench_covariance_large, NULL, &result);
    fc_bench_result_print(&result);

    config.name = "Covariance 250x500 (Financial)";
    config.data_size = 250 * 500 * sizeof(double);
    fc_bench_run(&config, bench_covariance_financial, NULL, &result);
    fc_bench_result_print(&result);

    // Correlation benchmarks
    config.name = "Correlation 10x10";
    config.data_size = 10 * 10 * sizeof(double);
    fc_bench_run(&config, bench_correlation_small, NULL, &result);
    fc_bench_result_print(&result);

    config.name = "Correlation 500x500";
    config.data_size = 500 * 500 * sizeof(double);
    fc_bench_run(&config, bench_correlation_large, NULL, &result);
    fc_bench_result_print(&result);

    // Combined benchmark
    config.name = "Covariance+Correlation 250x100";
    config.data_size = 250 * 100 * sizeof(double);
    fc_bench_run(&config, bench_covariance_correlation_combined, NULL, &result);
    fc_bench_result_print(&result);

    // Spearman benchmarks
    printf("\n");
    config.name = "Spearman 100x5";
    config.data_size = 100 * 5 * sizeof(double);
    fc_bench_run(&config, bench_spearman_small, NULL, &result);
    fc_bench_result_print(&result);

    config.name = "Spearman 1000x20";
    config.data_size = 1000 * 20 * sizeof(double);
    fc_bench_run(&config, bench_spearman_medium, NULL, &result);
    fc_bench_result_print(&result);

    config.name = "Spearman 5000x50";
    config.data_size = 5000 * 50 * sizeof(double);
    fc_bench_run(&config, bench_spearman_large, NULL, &result);
    fc_bench_result_print(&result);

    config.name = "Spearman 250x100 (Financial)";
    config.data_size = 250 * 100 * sizeof(double);
    fc_bench_run(&config, bench_spearman_financial, NULL, &result);
    fc_bench_result_print(&result);
}
