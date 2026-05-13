/**
 * @file test_correlation.c
 * @brief Unit tests for correlation coefficient computation
 */

#include "test_framework.h"
#include "../include/correlation.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_TOLERANCE 1e-10

/* Test: Basic correlation - perfect positive correlation */
TEST(test_correlation_perfect_positive) {
    double x[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double y[] = {2.0, 4.0, 6.0, 8.0, 10.0};
    double corr;

    fc_status_t status = fc_stats_pearson_correlation_f64(x, y, 5, &corr);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(corr, 1.0, TEST_TOLERANCE);
}

/* Test: Basic correlation - perfect negative correlation */
TEST(test_correlation_perfect_negative) {
    double x[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double y[] = {10.0, 8.0, 6.0, 4.0, 2.0};
    double corr;

    fc_status_t status = fc_stats_pearson_correlation_f64(x, y, 5, &corr);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(corr, -1.0, TEST_TOLERANCE);
}

/* Test: Basic correlation - no correlation */
TEST(test_correlation_zero) {
    double x[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double y[] = {3.0, 3.0, 3.0, 3.0, 3.0};
    double corr;

    fc_status_t status = fc_stats_pearson_correlation_f64(x, y, 5, &corr);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(corr, 0.0, TEST_TOLERANCE);
}

/* Test: Correlation with known value */
TEST(test_correlation_known_value) {
    /* Data with known correlation coefficient */
    double x[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double y[] = {2.5, 3.5, 4.0, 5.5, 6.0, 7.5};
    double corr;

    /* Expected correlation computed externally: ~0.9905 */
    fc_status_t status = fc_stats_pearson_correlation_f64(x, y, 6, &corr);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(corr, 0.990478003842680, 1e-6);
}

/* Test: Correlation matrix - 2x2 identity case */
TEST(test_correlation_matrix_identity) {
    /* Two identical variables */
    double data[] = {
        1.0, 1.0,  /* sample 0: var0=1.0, var1=1.0 */
        2.0, 2.0,  /* sample 1: var0=2.0, var1=2.0 */
        3.0, 3.0,  /* sample 2: var0=3.0, var1=3.0 */
        4.0, 4.0   /* sample 3: var0=4.0, var1=4.0 */
    };
    double corr_matrix[4];

    fc_status_t status = fc_stats_pearson_correlation_matrix_f64(data, 4, 2, corr_matrix);
    ASSERT_EQ(status, FC_OK);

    /* Diagonal should be 1.0 */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[3], 1.0, TEST_TOLERANCE);

    /* Off-diagonal should be 1.0 (perfect correlation) */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[1], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[2], 1.0, TEST_TOLERANCE);
}

/* Test: Correlation matrix - 3x3 with known values */
TEST(test_correlation_matrix_3x3) {
    /* Three variables with different correlations */
    double data[] = {
        1.0, 2.0, 5.0,   /* sample 0 */
        2.0, 4.0, 4.0,   /* sample 1 */
        3.0, 6.0, 3.0,   /* sample 2 */
        4.0, 8.0, 2.0,   /* sample 3 */
        5.0, 10.0, 1.0   /* sample 4 */
    };
    double corr_matrix[9];

    fc_status_t status = fc_stats_pearson_correlation_matrix_f64(data, 5, 3, corr_matrix);
    ASSERT_EQ(status, FC_OK);

    /* Diagonal should be 1.0 */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[4], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[8], 1.0, TEST_TOLERANCE);

    /* Var0 and Var1 should have perfect positive correlation */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[1], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[3], 1.0, TEST_TOLERANCE);

    /* Var0 and Var2 should have perfect negative correlation */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[2], -1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[6], -1.0, TEST_TOLERANCE);

    /* Var1 and Var2 should have perfect negative correlation */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[5], -1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[7], -1.0, TEST_TOLERANCE);
}

/* Test: Correlation matrix - symmetry */
TEST(test_correlation_matrix_symmetry) {
    /* Random data */
    double data[] = {
        1.5, 2.3, 3.1,
        2.1, 3.5, 2.8,
        3.2, 4.1, 3.5,
        4.0, 5.2, 4.2
    };
    double corr_matrix[9];

    fc_status_t status = fc_stats_pearson_correlation_matrix_f64(data, 4, 3, corr_matrix);
    ASSERT_EQ(status, FC_OK);

    /* Check symmetry */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[1], corr_matrix[3], TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[2], corr_matrix[6], TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[5], corr_matrix[7], TEST_TOLERANCE);
}

/* Test: Large dataset for SIMD optimization */
TEST(test_correlation_large_dataset) {
    const size_t n = 10000;
    double* x = (double*)malloc(n * sizeof(double));
    double* y = (double*)malloc(n * sizeof(double));

    /* Generate correlated data */
    for (size_t i = 0; i < n; i++) {
        x[i] = (double)i;
        y[i] = 2.0 * i + 1.0;  /* Perfect linear relationship */
    }

    double corr;
    fc_status_t status = fc_stats_pearson_correlation_f64(x, y, n, &corr);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(corr, 1.0, TEST_TOLERANCE);

    free(x);
    free(y);
}

/* Test: Error handling - NULL pointers */
TEST(test_correlation_null_pointers) {
    double x[] = {1.0, 2.0, 3.0};
    double y[] = {4.0, 5.0, 6.0};
    double corr;

    ASSERT_EQ(fc_stats_pearson_correlation_f64(NULL, y, 3, &corr), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_pearson_correlation_f64(x, NULL, 3, &corr), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_pearson_correlation_f64(x, y, 3, NULL), FC_ERR_INVALID_ARG);
}

/* Test: Error handling - insufficient data */
TEST(test_correlation_insufficient_data) {
    double x[] = {1.0};
    double y[] = {2.0};
    double corr;

    ASSERT_EQ(fc_stats_pearson_correlation_f64(x, y, 1, &corr), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_pearson_correlation_f64(x, y, 0, &corr), FC_ERR_INVALID_ARG);
}

/* Test: Error handling - NaN input */
TEST(test_correlation_nan_input) {
    double x[] = {1.0, 2.0, NAN, 4.0};
    double y[] = {2.0, 4.0, 6.0, 8.0};
    double corr;

    ASSERT_EQ(fc_stats_pearson_correlation_f64(x, y, 4, &corr), FC_ERR_NAN_INPUT);

    double x2[] = {1.0, 2.0, 3.0, 4.0};
    double y2[] = {2.0, NAN, 6.0, 8.0};
    ASSERT_EQ(fc_stats_pearson_correlation_f64(x2, y2, 4, &corr), FC_ERR_NAN_INPUT);
}

/* Test: Correlation matrix - NULL pointers */
TEST(test_correlation_matrix_null_pointers) {
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double corr_matrix[4];

    ASSERT_EQ(fc_stats_pearson_correlation_matrix_f64(NULL, 2, 2, corr_matrix), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_pearson_correlation_matrix_f64(data, 2, 2, NULL), FC_ERR_INVALID_ARG);
}

/* Test: Correlation matrix - invalid dimensions */
TEST(test_correlation_matrix_invalid_dimensions) {
    double data[] = {1.0, 2.0};
    double corr_matrix[1];

    ASSERT_EQ(fc_stats_pearson_correlation_matrix_f64(data, 1, 1, corr_matrix), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_pearson_correlation_matrix_f64(data, 2, 0, corr_matrix), FC_ERR_INVALID_ARG);
}

/* Test: Correlation with precomputed statistics */
TEST(test_correlation_matrix_precomputed) {
    double data[] = {
        1.0, 2.0,
        2.0, 4.0,
        3.0, 6.0,
        4.0, 8.0
    };
    double means[] = {2.5, 5.0};
    double stddevs[] = {1.29099444873581, 2.58198889747161};
    double corr_matrix[4];

    fc_status_t status = fc_stats_pearson_correlation_matrix_precomputed_f64(
        data, 4, 2, means, stddevs, corr_matrix
    );

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[3], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[1], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[2], 1.0, TEST_TOLERANCE);
}

/* Test: Correlation matrix - zero variance handling */
TEST(test_correlation_matrix_zero_variance) {
    double data[] = {
        1.0, 5.0,
        2.0, 5.0,
        3.0, 5.0,
        4.0, 5.0
    };
    double corr_matrix[4];

    fc_status_t status = fc_stats_pearson_correlation_matrix_f64(data, 4, 2, corr_matrix);
    ASSERT_EQ(status, FC_OK);

    /* Variable 1 has zero variance, so correlations should be 0.0 */
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[3], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[1], 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(corr_matrix[2], 0.0, TEST_TOLERANCE);
}

/* Register all correlation tests */
void register_correlation_tests(void) {
    RUN_TEST(test_correlation_perfect_positive);
    RUN_TEST(test_correlation_perfect_negative);
    RUN_TEST(test_correlation_zero);
    RUN_TEST(test_correlation_known_value);
    RUN_TEST(test_correlation_matrix_identity);
    RUN_TEST(test_correlation_matrix_3x3);
    RUN_TEST(test_correlation_matrix_symmetry);
    RUN_TEST(test_correlation_matrix_zero_variance);
    RUN_TEST(test_correlation_matrix_precomputed);
    RUN_TEST(test_correlation_large_dataset);
    RUN_TEST(test_correlation_null_pointers);
    RUN_TEST(test_correlation_insufficient_data);
    RUN_TEST(test_correlation_nan_input);
    RUN_TEST(test_correlation_matrix_null_pointers);
    RUN_TEST(test_correlation_matrix_invalid_dimensions);
}
