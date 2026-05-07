/**
 * @file test_covariance.c
 * @brief Unit tests for covariance and correlation computation
 */

#include "test_framework.h"
#include "../include/covariance.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define TEST_TOLERANCE 1e-10

TEST(test_covariance_basic) {
    /* Simple 2x2 case with known result
     * Data: [[1, 2], [3, 4], [5, 6]]
     * Mean: [3, 4]
     * Centered: [[-2, -2], [0, 0], [2, 2]]
     * Cov = [[-2, 0, 2]^T * [-2, 0, 2]] / 2 = [[8, 8], [8, 8]] / 2 = [[4, 4], [4, 4]]
     */
    double data[] = {1, 2, 3, 4, 5, 6};
    double cov[4];

    fc_status_t status = fc_stats_covariance_f64(cov, data, 3, 2, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(cov[0], 4.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(cov[1], 4.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(cov[2], 4.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(cov[3], 4.0, TEST_TOLERANCE);
}

TEST(test_covariance_population) {
    /* Test population covariance (divide by n instead of n-1) */
    double data[] = {1, 2, 3, 4, 5, 6};
    double cov[4];

    fc_status_t status = fc_stats_covariance_f64(cov, data, 3, 2, 0);
    ASSERT_EQ(status, FC_OK);

    /* Population covariance should be 2/3 of sample covariance */
    FC_TEST_ASSERT_DOUBLE_EQ(cov[0], 8.0 / 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(cov[1], 8.0 / 3.0, TEST_TOLERANCE);
}

TEST(test_covariance_identity) {
    /* Uncorrelated variables should have zero covariance
     * Data: [[1, 0], [0, 1], [-1, 0], [0, -1]]
     */
    double data[] = {1, 0, 0, 1, -1, 0, 0, -1};
    double cov[4];

    fc_status_t status = fc_stats_covariance_f64(cov, data, 4, 2, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(cov[1], 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(cov[2], 0.0, TEST_TOLERANCE);
}

TEST(test_covariance_symmetric) {
    /* Covariance matrix should be symmetric */
    double data[30];
    for (int i = 0; i < 30; i++) {
        data[i] = (double) (i % 7) + 0.5 * (i % 3);
    }

    double cov[9];
    fc_status_t status = fc_stats_covariance_f64(cov, data, 10, 3, 1);
    ASSERT_EQ(status, FC_OK);

    /* Check symmetry */
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            FC_TEST_ASSERT_DOUBLE_EQ(cov[i * 3 + j], cov[j * 3 + i], TEST_TOLERANCE);
        }
    }
}

TEST(test_correlation_basic) {
    /* Perfect positive correlation */
    double cov[] = {4.0, 4.0, 4.0, 4.0};
    double corr[4];

    fc_status_t status = fc_stats_correlation_f64(corr, cov, 2);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(corr[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr[1], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr[2], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr[3], 1.0, TEST_TOLERANCE);
}

TEST(test_correlation_uncorrelated) {
    /* Zero covariance */
    double cov[] = {1.0, 0.0, 0.0, 1.0};
    double corr[4];

    fc_status_t status = fc_stats_correlation_f64(corr, cov, 2);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(corr[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr[1], 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr[2], 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr[3], 1.0, TEST_TOLERANCE);
}

TEST(test_correlation_range) {
    /* Correlation coefficients should be in [-1, 1] */
    double data[100];
    for (int i = 0; i < 100; i++) {
        data[i] = sin((double) i * 0.1) + cos((double) i * 0.2);
    }

    double cov[25];
    double corr[25];

    fc_status_t status = fc_stats_covariance_correlation_f64(cov, corr, data, 20, 5, 1);
    ASSERT_EQ(status, FC_OK);

    /* Check correlation range */
    for (size_t i = 0; i < 5; i++) {
        for (size_t j = 0; j < 5; j++) {
            ASSERT_TRUE(corr[i * 5 + j] >= -1.0 - TEST_TOLERANCE);
            ASSERT_TRUE(corr[i * 5 + j] <= 1.0 + TEST_TOLERANCE);
        }
    }

    /* Check diagonal is 1.0 */
    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(corr[i * 5 + i], 1.0, TEST_TOLERANCE);
    }
}

TEST(test_covariance_null_input) {
    double data[6] = {1, 2, 3, 4, 5, 6};
    double cov[4];

    fc_status_t status = fc_stats_covariance_f64(NULL, data, 3, 2, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_covariance_f64(cov, NULL, 3, 2, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_covariance_invalid_dimensions) {
    double data[6] = {1, 2, 3, 4, 5, 6};
    double cov[4];

    fc_status_t status = fc_stats_covariance_f64(cov, data, 0, 2, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_covariance_f64(cov, data, 3, 0, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_covariance_insufficient_samples) {
    double data[2] = {1, 2};
    double cov[1];

    fc_status_t status = fc_stats_covariance_f64(cov, data, 1, 1, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    /* Population covariance should work with n=1 */
    status = fc_stats_covariance_f64(cov, data, 1, 1, 0);
    ASSERT_EQ(status, FC_OK);
}

TEST(test_covariance_large_matrix) {
    /* Test with larger matrix (50 samples, 10 variables) */
    const size_t n_samples = 50;
    const size_t n_vars    = 10;

    double* data = (double*) malloc(n_samples * n_vars * sizeof(double));
    double* cov  = (double*) malloc(n_vars * n_vars * sizeof(double));

    /* Generate test data */
    for (size_t i = 0; i < n_samples * n_vars; i++) {
        data[i] = sin((double) i * 0.1) + cos((double) i * 0.05);
    }

    fc_status_t status = fc_stats_covariance_f64(cov, data, n_samples, n_vars, 1);
    ASSERT_EQ(status, FC_OK);

    /* Verify symmetry */
    for (size_t i = 0; i < n_vars; i++) {
        for (size_t j = 0; j < n_vars; j++) {
            FC_TEST_ASSERT_DOUBLE_EQ(cov[i * n_vars + j], cov[j * n_vars + i], TEST_TOLERANCE);
        }
    }

    /* Verify diagonal is positive */
    for (size_t i = 0; i < n_vars; i++) {
        ASSERT_TRUE(cov[i * n_vars + i] >= 0.0);
    }

    free(data);
    free(cov);
}

TEST(test_covariance_constant_variable) {
    /* One variable is constant (zero variance) */
    double data[] = {1, 5, 2, 5, 3, 5, 4, 5};
    double cov[4];

    fc_status_t status = fc_stats_covariance_f64(cov, data, 4, 2, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(cov[3], 0.0, TEST_TOLERANCE);
}

void register_covariance_tests(void) {
    RUN_TEST(test_covariance_basic);
    RUN_TEST(test_covariance_population);
    RUN_TEST(test_covariance_identity);
    RUN_TEST(test_covariance_symmetric);
    RUN_TEST(test_correlation_basic);
    RUN_TEST(test_correlation_uncorrelated);
    RUN_TEST(test_correlation_range);
    RUN_TEST(test_covariance_null_input);
    RUN_TEST(test_covariance_invalid_dimensions);
    RUN_TEST(test_covariance_insufficient_samples);
    RUN_TEST(test_covariance_large_matrix);
    RUN_TEST(test_covariance_constant_variable);
}
