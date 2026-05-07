/**
 * @file test_quantile.c
 * @brief Unit tests for quantile computation
 */

#include "test_framework.h"
#include "../include/quantile.h"
#include <math.h>
#include <float.h>
#include <string.h>

#define TEST_TOLERANCE 1e-12

/*
 * Test: Basic quantile computation
 */

TEST(test_quantile_median_odd) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0, TEST_TOLERANCE);
}

TEST(test_quantile_median_even) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    /* Median of even-length array: interpolation between 3.0 and 4.0 */
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.5, TEST_TOLERANCE);
}

TEST(test_quantile_q1) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.25, &result);

    ASSERT_EQ(status, FC_OK);
    /* Q1 at position 0.25 * 7 = 1.75, interpolate between index 1 and 2 */
    FC_TEST_ASSERT_DOUBLE_EQ(result, 2.75, TEST_TOLERANCE);
}

TEST(test_quantile_q3) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.75, &result);

    ASSERT_EQ(status, FC_OK);
    /* Q3 at position 0.75 * 7 = 5.25, interpolate between index 5 and 6 */
    FC_TEST_ASSERT_DOUBLE_EQ(result, 6.25, TEST_TOLERANCE);
}

TEST(test_quantile_min) {
    double data[] = {5.0, 2.0, 8.0, 1.0, 9.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.0, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 1.0, TEST_TOLERANCE);
}

TEST(test_quantile_max) {
    double data[] = {5.0, 2.0, 8.0, 1.0, 9.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 1.0, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 9.0, TEST_TOLERANCE);
}

/*
 * Test: Edge cases
 */

TEST(test_quantile_single_element) {
    double data[] = {42.0};
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, 1, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 42.0, TEST_TOLERANCE);
}

TEST(test_quantile_two_elements) {
    double data[] = {1.0, 2.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 1.5, TEST_TOLERANCE);
}

TEST(test_quantile_all_same) {
    double data[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 5.0, TEST_TOLERANCE);
}

TEST(test_quantile_unsorted) {
    double data[] = {9.0, 1.0, 5.0, 3.0, 7.0, 2.0, 8.0, 4.0, 6.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 5.0, TEST_TOLERANCE);
}

/*
 * Test: Error handling
 */

TEST(test_quantile_null_data) {
    double result;
    fc_status_t status = fc_stats_quantile_f64(NULL, 10, 0.5, &result);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantile_null_result) {
    double data[] = {1.0, 2.0, 3.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, NULL);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantile_zero_length) {
    double data[] = {1.0};
    double result;
    fc_status_t status = fc_stats_quantile_f64(data, 0, 0.5, &result);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantile_invalid_q_negative) {
    double data[] = {1.0, 2.0, 3.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;
    fc_status_t status = fc_stats_quantile_f64(data, n, -0.1, &result);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantile_invalid_q_greater_than_one) {
    double data[] = {1.0, 2.0, 3.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;
    fc_status_t status = fc_stats_quantile_f64(data, n, 1.1, &result);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantile_nan_in_data) {
    double data[] = {1.0, 2.0, NAN, 4.0, 5.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;
    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantile_inf_in_data) {
    double data[] = {1.0, 2.0, INFINITY, 4.0, 5.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;
    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantile_neg_inf_in_data) {
    double data[] = {1.0, 2.0, -INFINITY, 4.0, 5.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;
    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

/*
 * Test: Multiple quantiles
 */

TEST(test_quantiles_multiple) {
    double data[]      = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    size_t n           = sizeof(data) / sizeof(data[0]);
    double quantiles[] = {0.25, 0.5, 0.75};
    double results[3];

    fc_status_t status = fc_stats_quantiles_f64(data, n, quantiles, 3, results);

    ASSERT_EQ(status, FC_OK);
    /* Q1: 0.25 * 9 = 2.25, interpolate between index 2 and 3 */
    FC_TEST_ASSERT_DOUBLE_EQ(results[0], 3.25, TEST_TOLERANCE);
    /* Q2: 0.5 * 9 = 4.5, interpolate between index 4 and 5 */
    FC_TEST_ASSERT_DOUBLE_EQ(results[1], 5.5, TEST_TOLERANCE);
    /* Q3: 0.75 * 9 = 6.75, interpolate between index 6 and 7 */
    FC_TEST_ASSERT_DOUBLE_EQ(results[2], 7.75, TEST_TOLERANCE);
}

TEST(test_quantiles_null_quantiles) {
    double data[] = {1.0, 2.0, 3.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double results[3];
    fc_status_t status = fc_stats_quantiles_f64(data, n, NULL, 3, results);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_quantiles_null_results) {
    double data[]      = {1.0, 2.0, 3.0};
    size_t n           = sizeof(data) / sizeof(data[0]);
    double quantiles[] = {0.25, 0.5, 0.75};
    fc_status_t status = fc_stats_quantiles_f64(data, n, quantiles, 3, NULL);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

/*
 * Test: Median convenience function
 */

TEST(test_median_basic) {
    double data[] = {5.0, 2.0, 8.0, 1.0, 9.0, 3.0, 7.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_median_f64(data, n, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 5.0, TEST_TOLERANCE);
}

TEST(test_median_even_length) {
    double data[] = {1.0, 2.0, 3.0, 4.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_median_f64(data, n, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 2.5, TEST_TOLERANCE);
}

/*
 * Test: Quartiles convenience function
 */

TEST(test_quartiles_basic) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double q1, q2, q3;

    fc_status_t status = fc_stats_quartiles_f64(data, n, &q1, &q2, &q3);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(q1, 3.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(q2, 5.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(q3, 7.75, TEST_TOLERANCE);
}

TEST(test_quartiles_null_output) {
    double data[] = {1.0, 2.0, 3.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double q1, q2;
    fc_status_t status = fc_stats_quartiles_f64(data, n, &q1, &q2, NULL);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

/*
 * Test: Large dataset
 */

TEST(test_quantile_large_dataset) {
    const size_t n = 10000;
    double* data   = malloc(n * sizeof(double));
    ASSERT_NE(data, NULL);

    /* Generate sequential data */
    for (size_t i = 0; i < n; i++) {
        data[i] = (double) i;
    }

    double result;
    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    /* Median should be around 4999.5 */
    FC_TEST_ASSERT_DOUBLE_EQ(result, 4999.5, TEST_TOLERANCE);

    free(data);
}

/*
 * Test: Negative values
 */

TEST(test_quantile_negative_values) {
    double data[] = {-5.0, -3.0, -1.0, 1.0, 3.0, 5.0};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 0.0, TEST_TOLERANCE);
}

/*
 * Test: Floating-point precision
 */

TEST(test_quantile_precision) {
    double data[] = {1.0 + 1e-10, 2.0 + 1e-10, 3.0 + 1e-10, 4.0 + 1e-10, 5.0 + 1e-10};
    size_t n      = sizeof(data) / sizeof(data[0]);
    double result;

    fc_status_t status = fc_stats_quantile_f64(data, n, 0.5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0 + 1e-10, TEST_TOLERANCE);
}

/*
 * Test suite registration
 */

void register_quantile_tests(void) {
    RUN_TEST(test_quantile_median_odd);
    RUN_TEST(test_quantile_median_even);
    RUN_TEST(test_quantile_q1);
    RUN_TEST(test_quantile_q3);
    RUN_TEST(test_quantile_min);
    RUN_TEST(test_quantile_max);

    RUN_TEST(test_quantile_single_element);
    RUN_TEST(test_quantile_two_elements);
    RUN_TEST(test_quantile_all_same);
    RUN_TEST(test_quantile_unsorted);

    RUN_TEST(test_quantile_null_data);
    RUN_TEST(test_quantile_null_result);
    RUN_TEST(test_quantile_zero_length);
    RUN_TEST(test_quantile_invalid_q_negative);
    RUN_TEST(test_quantile_invalid_q_greater_than_one);
    RUN_TEST(test_quantile_nan_in_data);
    RUN_TEST(test_quantile_inf_in_data);
    RUN_TEST(test_quantile_neg_inf_in_data);

    RUN_TEST(test_quantiles_multiple);
    RUN_TEST(test_quantiles_null_quantiles);
    RUN_TEST(test_quantiles_null_results);

    RUN_TEST(test_median_basic);
    RUN_TEST(test_median_even_length);

    RUN_TEST(test_quartiles_basic);
    RUN_TEST(test_quartiles_null_output);

    RUN_TEST(test_quantile_large_dataset);
    RUN_TEST(test_quantile_negative_values);
    RUN_TEST(test_quantile_precision);
}
