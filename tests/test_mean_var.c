/**
 * @file test_mean_var.c
 * @brief Unit tests for mean and variance computation
 */

#include "test_framework.h"
#include "../include/mean_var.h"
#include <math.h>
#include <float.h>

/* Test tolerance for floating-point comparisons */
#define TEST_TOLERANCE 1e-12

/*
 * Test: Basic mean computation
*/

TEST(test_mean_basic) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double mean;

    fc_status_t status = fc_stats_mean_f64(data, n, &mean);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 3.0, TEST_TOLERANCE);
}

TEST(test_mean_single_element) {
    double data[] = {42.0};
    double mean;

    fc_status_t status = fc_stats_mean_f64(data, 1, &mean);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 42.0, TEST_TOLERANCE);
}

TEST(test_mean_negative_values) {
    double data[] = {-5.0, -3.0, -1.0, 1.0, 3.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double mean;

    fc_status_t status = fc_stats_mean_f64(data, n, &mean);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 0.0, TEST_TOLERANCE);
}

TEST(test_mean_large_values) {
    double data[] = {1e10, 1e10 + 1.0, 1e10 + 2.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double mean;

    fc_status_t status = fc_stats_mean_f64(data, n, &mean);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 1e10 + 1.0, TEST_TOLERANCE);
}

/*
 * Test: Variance computation
*/

TEST(test_variance_sample) {
    double data[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double variance;

    fc_status_t status = fc_stats_variance_f64(data, n, &variance, 1);

    ASSERT_EQ(status, FC_OK);
    /* Expected sample variance: 4.571428571428571 */
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 4.571428571428571, TEST_TOLERANCE);
}

TEST(test_variance_population) {
    double data[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double variance;

    fc_status_t status = fc_stats_variance_f64(data, n, &variance, 0);

    ASSERT_EQ(status, FC_OK);
    /* Expected population variance: 4.0 */
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 4.0, TEST_TOLERANCE);
}

TEST(test_variance_constant) {
    double data[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double variance;

    fc_status_t status = fc_stats_variance_f64(data, n, &variance, 1);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.0, TEST_TOLERANCE);
}

/*
 * Test: Mean and variance together
*/

TEST(test_mean_variance_combined) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double mean, variance;

    fc_status_t status = fc_stats_mean_variance_f64(data, n, &mean, &variance, 1);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 2.5, TEST_TOLERANCE);
}

/*
 * Test: Standard deviation
*/

TEST(test_stddev_basic) {
    double data[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double stddev;

    fc_status_t status = fc_stats_stddev_f64(data, n, &stddev, 1);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev, 2.138089935299395, TEST_TOLERANCE);
}

/*
 * Test: Batch operations
*/

TEST(test_mean_batch) {
    double data1[] = {1.0, 2.0, 3.0};
    double data2[] = {4.0, 5.0, 6.0};
    double data3[] = {7.0, 8.0, 9.0};
    const double* data[] = {data1, data2, data3};
    double means[3];

    fc_status_t status = fc_stats_mean_batch_f64(data, 3, 3, means);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(means[0], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(means[1], 5.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(means[2], 8.0, TEST_TOLERANCE);
}

TEST(test_variance_batch) {
    double data1[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double data2[] = {2.0, 4.0, 6.0, 8.0, 10.0};
    const double* data[] = {data1, data2};
    double variances[2];

    fc_status_t status = fc_stats_variance_batch_f64(data, 5, 2, variances, 1);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[0], 2.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[1], 10.0, TEST_TOLERANCE);
}

TEST(test_mean_variance_batch) {
    double data1[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double data2[] = {10.0, 20.0, 30.0, 40.0, 50.0};
    const double* data[] = {data1, data2};
    double means[2];
    double variances[2];

    fc_status_t status = fc_stats_mean_variance_batch_f64(
        data, 5, 2, means, variances, 1);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(means[0], 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(means[1], 30.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[0], 2.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[1], 250.0, TEST_TOLERANCE);
}

/*
 * Test: Large dataset (SIMD optimization)
*/

TEST(test_mean_large_dataset) {
    const size_t n = 10000;
    double* data = (double*)malloc(n * sizeof(double));
    ASSERT_NOT_NULL(data);

    /* Generate data: 0, 1, 2, ..., 9999 */
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)i;
    }

    double mean;
    fc_status_t status = fc_stats_mean_f64(data, n, &mean);

    ASSERT_EQ(status, FC_OK);
    /* Expected mean: (0 + 9999) / 2 = 4999.5 */
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 4999.5, TEST_TOLERANCE);

    free(data);
}

TEST(test_variance_large_dataset) {
    const size_t n = 1000;
    double* data = (double*)malloc(n * sizeof(double));
    ASSERT_NOT_NULL(data);

    /* Generate data: 0, 1, 2, ..., 999 */
    for (size_t i = 0; i < n; i++) {
        data[i] = (double)i;
    }

    double variance;
    fc_status_t status = fc_stats_variance_f64(data, n, &variance, 0);

    ASSERT_EQ(status, FC_OK);
    /* Expected population variance: (n^2 - 1) / 12 = 83333.25 */
    double expected = ((double)n * (double)n - 1.0) / 12.0;
    FC_TEST_ASSERT_DOUBLE_EQ(variance, expected, 1e-8);

    free(data);
}

/*
 * Test: Error handling
*/

TEST(test_mean_null_input) {
    double mean;
    fc_status_t status = fc_stats_mean_f64(NULL, 10, &mean);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_mean_null_output) {
    double data[] = {1.0, 2.0, 3.0};
    fc_status_t status = fc_stats_mean_f64(data, 3, NULL);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_mean_zero_length) {
    double data[] = {1.0};
    double mean;
    fc_status_t status = fc_stats_mean_f64(data, 0, &mean);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_variance_insufficient_samples) {
    double data[] = {1.0};
    double variance;
    fc_status_t status = fc_stats_variance_f64(data, 1, &variance, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_mean_nan_input) {
    double data[] = {1.0, 2.0, NAN, 4.0, 5.0};
    double mean;
    fc_status_t status = fc_stats_mean_f64(data, 5, &mean);
    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

/*
 * Test: Numerical stability
*/

TEST(test_mean_numerical_stability) {
    /* Test with values that could cause catastrophic cancellation */
    double data[] = {1e15, 1e15 + 1.0, 1e15 + 2.0, 1e15 + 3.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double mean;

    fc_status_t status = fc_stats_mean_f64(data, n, &mean);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 1e15 + 1.5, 1e-6);
}

TEST(test_variance_numerical_stability) {
    /* Test Welford's algorithm stability */
    double data[] = {1e9, 1e9 + 1.0, 1e9 + 2.0, 1e9 + 3.0, 1e9 + 4.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double variance;

    fc_status_t status = fc_stats_variance_f64(data, n, &variance, 1);

    ASSERT_EQ(status, FC_OK);
    /* Expected sample variance: 2.5 */
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 2.5, 1e-6);
}

/*
 * Test suite registration
*/

void register_mean_var_tests(void) {
    /* Basic mean tests */
    RUN_TEST(test_mean_basic);
    RUN_TEST(test_mean_single_element);
    RUN_TEST(test_mean_negative_values);
    RUN_TEST(test_mean_large_values);

    /* Variance tests */
    RUN_TEST(test_variance_sample);
    RUN_TEST(test_variance_population);
    RUN_TEST(test_variance_constant);

    /* Combined tests */
    RUN_TEST(test_mean_variance_combined);

    /* Standard deviation tests */
    RUN_TEST(test_stddev_basic);

    /* Batch tests */
    RUN_TEST(test_mean_batch);
    RUN_TEST(test_variance_batch);
    RUN_TEST(test_mean_variance_batch);

    /* Large dataset tests */
    RUN_TEST(test_mean_large_dataset);
    RUN_TEST(test_variance_large_dataset);

    /* Error handling tests */
    RUN_TEST(test_mean_null_input);
    RUN_TEST(test_mean_null_output);
    RUN_TEST(test_mean_zero_length);
    RUN_TEST(test_variance_insufficient_samples);
    RUN_TEST(test_mean_nan_input);

    /* Numerical stability tests */
    RUN_TEST(test_mean_numerical_stability);
    RUN_TEST(test_variance_numerical_stability);
}
