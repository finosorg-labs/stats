/**
 * @file test_rolling.c
 * @brief Unit tests for rolling window statistics
 */

#include "../include/rolling.h"
#include "test_framework.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_TOLERANCE 1e-10

TEST(test_rolling_mean_basic) {
    double data[]     = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 3;
    fc_status_t status = fc_stats_rolling_mean_f64(data, n, window, output);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 1.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 4.0, TEST_TOLERANCE);
}

TEST(test_rolling_mean_window_one) {
    double data[]     = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 1;
    fc_status_t status = fc_stats_rolling_mean_f64(data, n, window, output);

    ASSERT_EQ(status, FC_OK);
    for (size_t i = 0; i < n; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(output[i], data[i], TEST_TOLERANCE);
    }
}

TEST(test_rolling_mean_full_window) {
    double data[]     = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 5;
    fc_status_t status = fc_stats_rolling_mean_f64(data, n, window, output);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 1.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 2.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 3.0, TEST_TOLERANCE);
}

TEST(test_rolling_variance_basic) {
    double data[]     = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 3;
    fc_status_t status = fc_stats_rolling_variance_f64(data, n, window, output, 1);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 0.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 1.0, TEST_TOLERANCE);
}

TEST(test_rolling_variance_population) {
    double data[]     = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 3;
    fc_status_t status = fc_stats_rolling_variance_f64(data, n, window, output, 0);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 2.0 / 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 2.0 / 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 2.0 / 3.0, TEST_TOLERANCE);
}

TEST(test_rolling_stddev_basic) {
    double data[]     = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 3;
    fc_status_t status = fc_stats_rolling_stddev_f64(data, n, window, output, 1);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 1.0, TEST_TOLERANCE);
}

TEST(test_rolling_sum_basic) {
    double data[]     = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 3;
    fc_status_t status = fc_stats_rolling_sum_f64(data, n, window, output);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 6.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 9.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 12.0, TEST_TOLERANCE);
}

TEST(test_rolling_min_basic) {
    double data[]     = {5.0, 2.0, 8.0, 1.0, 9.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 3;
    fc_status_t status = fc_stats_rolling_min_f64(data, n, window, output);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 5.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 1.0, TEST_TOLERANCE);
}

TEST(test_rolling_max_basic) {
    double data[]     = {5.0, 2.0, 8.0, 1.0, 9.0};
    double output[5]  = {0};
    size_t n          = 5;
    size_t window     = 3;
    fc_status_t status = fc_stats_rolling_max_f64(data, n, window, output);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(output[0], 5.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[1], 5.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[2], 8.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[3], 8.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(output[4], 9.0, TEST_TOLERANCE);
}

TEST(test_rolling_mean_large_window) {
    size_t n      = 1000;
    size_t window = 250;
    double* data   = malloc(n * sizeof(double));
    double* output = malloc(n * sizeof(double));

    for (size_t i = 0; i < n; i++) {
        data[i] = (double) i;
    }

    fc_status_t status = fc_stats_rolling_mean_f64(data, n, window, output);
    ASSERT_EQ(status, FC_OK);

    /* Expected: mean of data[750..999] = (750+751+...+999)/250 = 874.5 */
    double expected = 0.0;
    for (size_t i = n - window; i < n; i++) {
        expected += data[i];
    }
    expected /= (double) window;

    FC_TEST_ASSERT_DOUBLE_EQ(output[n - 1], expected, TEST_TOLERANCE);

    free(data);
    free(output);
}

TEST(test_rolling_null_input) {
    double output[5]  = {0};
    fc_status_t status;

    status = fc_stats_rolling_mean_f64(NULL, 5, 3, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    status = fc_stats_rolling_mean_f64(data, 0, 3, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_rolling_mean_f64(data, 5, 3, NULL);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_rolling_invalid_window) {
    double data[5]    = {1.0, 2.0, 3.0, 4.0, 5.0};
    double output[5]  = {0};
    fc_status_t status;

    status = fc_stats_rolling_mean_f64(data, 5, 0, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_rolling_mean_f64(data, 5, 6, output);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_rolling_nan_input) {
    double data[5]    = {1.0, 2.0, NAN, 4.0, 5.0};
    double output[5]  = {0};
    fc_status_t status = fc_stats_rolling_mean_f64(data, 5, 3, output);

    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_rolling_mean_constant) {
    double data[10];
    double output[10] = {0};
    for (size_t i = 0; i < 10; i++) {
        data[i] = 42.0;
    }

    fc_status_t status = fc_stats_rolling_mean_f64(data, 10, 5, output);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 10; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(output[i], 42.0, TEST_TOLERANCE);
    }
}

TEST(test_rolling_variance_constant) {
    double data[10];
    double output[10] = {0};
    for (size_t i = 0; i < 10; i++) {
        data[i] = 42.0;
    }

    fc_status_t status = fc_stats_rolling_variance_f64(data, 10, 5, output, 1);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 1; i < 10; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(output[i], 0.0, TEST_TOLERANCE);
    }
}

void register_rolling_tests(void) {
    RUN_TEST(test_rolling_mean_basic);
    RUN_TEST(test_rolling_mean_window_one);
    RUN_TEST(test_rolling_mean_full_window);
    RUN_TEST(test_rolling_variance_basic);
    RUN_TEST(test_rolling_variance_population);
    RUN_TEST(test_rolling_stddev_basic);
    RUN_TEST(test_rolling_sum_basic);
    RUN_TEST(test_rolling_min_basic);
    RUN_TEST(test_rolling_max_basic);
    RUN_TEST(test_rolling_mean_large_window);
    RUN_TEST(test_rolling_null_input);
    RUN_TEST(test_rolling_invalid_window);
    RUN_TEST(test_rolling_nan_input);
    RUN_TEST(test_rolling_mean_constant);
    RUN_TEST(test_rolling_variance_constant);
}
