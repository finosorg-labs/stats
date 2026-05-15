/**
 * @file test_winsorize.c
 * @brief Unit tests for winsorization
 */

#include "test_framework.h"
#include "../include/winsorize.h"
#include <math.h>
#include <float.h>

#define TEST_TOLERANCE 1e-12

/*
 * Test: Basic winsorization
 */

TEST(test_winsorize_basic) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double output[10];

    fc_status_t status = fc_stats_winsorize_f64(data, n, 0.1, 0.9, output);

    ASSERT_EQ(status, FC_OK);

    /* 10th percentile = 1.9, 90th percentile = 9.1 */
    /* Values below 1.9 should be clamped to 1.9 */
    /* Values above 9.1 should be clamped to 9.1 */
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 1.9, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[8], 9.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[9], 9.1, TEST_TOLERANCE);
}

TEST(test_winsorize_symmetric_5pct) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
                     11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double output[20];

    fc_status_t status = fc_stats_winsorize_f64(data, n, 0.05, 0.95, output);

    ASSERT_EQ(status, FC_OK);

    /* 5th percentile = 1.95, 95th percentile = 19.05 */
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 1.95, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[19], 19.05, TEST_TOLERANCE);

    /* Middle values should be unchanged */
    FC_TEST_ASSERT_DOUBLE_EQ(output[10], 11.0, TEST_TOLERANCE);
}

TEST(test_winsorize_with_outliers) {
    double data[] = {-100.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 100.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double output[10];

    fc_status_t status = fc_stats_winsorize_f64(data, n, 0.1, 0.9, output);

    ASSERT_EQ(status, FC_OK);

    /* Extreme outliers should be clamped */
    ASSERT_TRUE(output[0] > -100.0);
    ASSERT_TRUE(output[9] < 100.0);
}

TEST(test_winsorize_no_change) {
    double data[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double output[5];

    fc_status_t status = fc_stats_winsorize_f64(data, n, 0.1, 0.9, output);

    ASSERT_EQ(status, FC_OK);

    /* All values should remain 5.0 */
    for (size_t i = 0; i < n; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(output[i], 5.0, TEST_TOLERANCE);
    }
}

TEST(test_winsorize_single_element) {
    double data[] = {42.0};
    double output[1];

    fc_status_t status = fc_stats_winsorize_f64(data, 1, 0.1, 0.9, output);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 42.0, TEST_TOLERANCE);
}

TEST(test_winsorize_two_elements) {
    double data[] = {1.0, 10.0};
    double output[2];

    fc_status_t status = fc_stats_winsorize_f64(data, 2, 0.25, 0.75, output);

    ASSERT_EQ(status, FC_OK);

    /* 25th percentile = 3.25, 75th percentile = 7.75 */
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 3.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 7.75, TEST_TOLERANCE);
}

/*
 * Test: In-place winsorization
 */

TEST(test_winsorize_inplace_basic) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    size_t n = sizeof(data) / sizeof(data[0]);

    fc_status_t status = fc_stats_winsorize_inplace_f64(data, n, 0.1, 0.9);

    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(data[0], 1.9, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(data[9], 9.1, TEST_TOLERANCE);
}

TEST(test_winsorize_inplace_preserves_middle) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    size_t n = sizeof(data) / sizeof(data[0]);

    fc_status_t status = fc_stats_winsorize_inplace_f64(data, n, 0.2, 0.8);

    ASSERT_EQ(status, FC_OK);

    /* Middle values should be unchanged */
    FC_TEST_ASSERT_DOUBLE_EQ(data[4], 5.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(data[5], 6.0, TEST_TOLERANCE);
}

/*
 * Test: Batch winsorization
 */

TEST(test_winsorize_batch_basic) {
    double data1[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double data2[] = {10.0, 20.0, 30.0, 40.0, 50.0};
    double data3[] = {-5.0, -3.0, 0.0, 3.0, 5.0};

    const double* data[] = {data1, data2, data3};

    double output1[5], output2[5], output3[5];
    double* output[] = {output1, output2, output3};

    size_t n = 5;
    size_t num_arrays = 3;

    fc_status_t status = fc_stats_winsorize_batch_f64(data, n, num_arrays, 0.2, 0.8, output);

    ASSERT_EQ(status, FC_OK);

    /* Check first array */
    FC_TEST_ASSERT_DOUBLE_EQ(output1[0], 1.8, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output1[4], 4.2, TEST_TOLERANCE);

    /* Check second array */
    FC_TEST_ASSERT_DOUBLE_EQ(output2[0], 18.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output2[4], 42.0, TEST_TOLERANCE);
}

TEST(test_winsorize_batch_independent) {
    double data1[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double data2[] = {100.0, 200.0, 300.0, 400.0, 500.0};

    const double* data[] = {data1, data2};

    double output1[5], output2[5];
    double* output[] = {output1, output2};

    size_t n = 5;
    size_t num_arrays = 2;

    fc_status_t status = fc_stats_winsorize_batch_f64(data, n, num_arrays, 0.1, 0.9, output);

    ASSERT_EQ(status, FC_OK);

    /* Each array should be winsorized independently */
    /* data1: 10th pct = 1.4, 90th pct = 4.6 */
    FC_TEST_ASSERT_DOUBLE_EQ(output1[0], 1.4, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output1[4], 4.6, TEST_TOLERANCE);

    /* data2: 10th pct = 140, 90th pct = 460 */
    FC_TEST_ASSERT_DOUBLE_EQ(output2[0], 140.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output2[4], 460.0, TEST_TOLERANCE);
}

/*
 * Test: Error handling
 */

TEST(test_winsorize_null_input) {
    double output[10];
    fc_status_t status = fc_stats_winsorize_f64(NULL, 10, 0.1, 0.9, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_winsorize_null_output) {
    double data[] = {1.0, 2.0, 3.0};
    fc_status_t status = fc_stats_winsorize_f64(data, 3, 0.1, 0.9, NULL);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_winsorize_zero_length) {
    double data[] = {1.0};
    double output[1];
    fc_status_t status = fc_stats_winsorize_f64(data, 0, 0.1, 0.9, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_winsorize_invalid_percentiles) {
    double data[] = {1.0, 2.0, 3.0};
    double output[3];

    /* lower >= upper */
    fc_status_t status = fc_stats_winsorize_f64(data, 3, 0.9, 0.1, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    /* lower == upper */
    status = fc_stats_winsorize_f64(data, 3, 0.5, 0.5, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    /* out of range */
    status = fc_stats_winsorize_f64(data, 3, -0.1, 0.9, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_winsorize_f64(data, 3, 0.1, 1.1, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_winsorize_nan_input) {
    double data[] = {1.0, 2.0, NAN, 4.0, 5.0};
    double output[5];

    fc_status_t status = fc_stats_winsorize_f64(data, 5, 0.1, 0.9, output);
    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_winsorize_inplace_null_input) {
    fc_status_t status = fc_stats_winsorize_inplace_f64(NULL, 10, 0.1, 0.9);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_winsorize_batch_null_arrays) {
    double data1[] = {1.0, 2.0, 3.0};
    const double* data[] = {data1, NULL};

    double output1[3], output2[3];
    double* output[] = {output1, output2};

    fc_status_t status = fc_stats_winsorize_batch_f64(data, 3, 2, 0.1, 0.9, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_winsorize_batch_zero_arrays) {
    double data1[] = {1.0, 2.0, 3.0};
    const double* data[] = {data1};

    double output1[3];
    double* output[] = {output1};

    fc_status_t status = fc_stats_winsorize_batch_f64(data, 3, 0, 0.1, 0.9, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

/*
 * Test: Edge cases
 */

TEST(test_winsorize_extreme_percentiles) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5];

    /* Very narrow range */
    fc_status_t status = fc_stats_winsorize_f64(data, 5, 0.4, 0.6, output);
    ASSERT_EQ(status, FC_OK);

    /* All values should be clamped to middle range */
    for (size_t i = 0; i < 5; i++) {
        ASSERT_TRUE(output[i] >= 2.6 && output[i] <= 3.4);
    }
}

TEST(test_winsorize_wide_percentiles) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5];

    /* Very wide range - should change almost nothing */
    fc_status_t status = fc_stats_winsorize_f64(data, 5, 0.01, 0.99, output);
    ASSERT_EQ(status, FC_OK);

    /* Most values should be unchanged */
    for (size_t i = 1; i < 4; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(output[i], data[i], TEST_TOLERANCE);
    }
}

TEST(test_winsorize_negative_values) {
    double data[] = {-10.0, -5.0, -3.0, -1.0, 0.0, 1.0, 3.0, 5.0, 10.0};
    size_t n = sizeof(data) / sizeof(data[0]);
    double output[9];

    fc_status_t status = fc_stats_winsorize_f64(data, n, 0.1, 0.9, output);

    ASSERT_EQ(status, FC_OK);

    /* Check that negative values are handled correctly */
    ASSERT_TRUE(output[0] > -10.0);
    ASSERT_TRUE(output[8] < 10.0);
}

TEST(test_winsorize_large_array) {
    const size_t n = 10000;
    double* data = (double*) malloc(n * sizeof(double));
    double* output = (double*) malloc(n * sizeof(double));

    ASSERT_TRUE(data != NULL && output != NULL);

    /* Fill with sequential values */
    for (size_t i = 0; i < n; i++) {
        data[i] = (double) i;
    }

    fc_status_t status = fc_stats_winsorize_f64(data, n, 0.05, 0.95, output);

    ASSERT_EQ(status, FC_OK);

    /* Check that extreme values are clamped */
    ASSERT_TRUE(output[0] > 0.0);
    ASSERT_TRUE(output[n-1] < (double)(n-1));

    /* Check that middle values are unchanged */
    FC_TEST_ASSERT_DOUBLE_EQ(output[n/2], (double)(n/2), TEST_TOLERANCE);

    free(data);
    free(output);
}

void register_winsorize_tests(void) {
    RUN_TEST(test_winsorize_basic);
    RUN_TEST(test_winsorize_symmetric_5pct);
    RUN_TEST(test_winsorize_with_outliers);
    RUN_TEST(test_winsorize_no_change);
    RUN_TEST(test_winsorize_single_element);
    RUN_TEST(test_winsorize_two_elements);

    RUN_TEST(test_winsorize_inplace_basic);
    RUN_TEST(test_winsorize_inplace_preserves_middle);

    RUN_TEST(test_winsorize_batch_basic);
    RUN_TEST(test_winsorize_batch_independent);

    RUN_TEST(test_winsorize_null_input);
    RUN_TEST(test_winsorize_null_output);
    RUN_TEST(test_winsorize_zero_length);
    RUN_TEST(test_winsorize_invalid_percentiles);
    RUN_TEST(test_winsorize_nan_input);
    RUN_TEST(test_winsorize_inplace_null_input);
    RUN_TEST(test_winsorize_batch_null_arrays);
    RUN_TEST(test_winsorize_batch_zero_arrays);

    RUN_TEST(test_winsorize_extreme_percentiles);
    RUN_TEST(test_winsorize_wide_percentiles);
    RUN_TEST(test_winsorize_negative_values);
    RUN_TEST(test_winsorize_large_array);
}
