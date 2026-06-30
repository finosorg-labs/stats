/**
 * @file test_weighted.c
 * @brief Unit tests for weighted statistical computations
 */

#include "test_framework.h"
#include "../include/weighted.h"
#include <math.h>
#include <stdint.h>

#define TEST_TOLERANCE 1e-12

TEST(test_weighted_sum_basic) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    double sum;

    fc_status_t status = fc_stats_weighted_sum_f64(data, weights, 3, &sum);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 9.0, TEST_TOLERANCE);
}

TEST(test_weighted_sum_zero_weights) {
    double data[] = {10.0, 20.0, 30.0};
    double weights[] = {0.0, 1.0, 0.0};
    double sum;

    fc_status_t status = fc_stats_weighted_sum_f64(data, weights, 3, &sum);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 20.0, TEST_TOLERANCE);
}

TEST(test_weighted_sum_all_zero_weights) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {0.0, 0.0, 0.0};
    double sum;

    fc_status_t status = fc_stats_weighted_sum_f64(data, weights, 3, &sum);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 0.0, TEST_TOLERANCE);
}

TEST(test_weighted_sum_negative_weight) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, -1.0, 2.0};
    double sum;

    fc_status_t status = fc_stats_weighted_sum_f64(data, weights, 3, &sum);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_sum_nan_data) {
    double data[] = {1.0, NAN, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    double sum;

    fc_status_t status = fc_stats_weighted_sum_f64(data, weights, 3, &sum);

    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_weighted_sum_nan_weight) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, NAN, 2.0};
    double sum;

    fc_status_t status = fc_stats_weighted_sum_f64(data, weights, 3, &sum);

    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_weighted_sum_null_inputs) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 1.0};
    double sum;

    ASSERT_EQ(fc_stats_weighted_sum_f64(NULL, weights, 3, &sum), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_sum_f64(data, NULL, 3, &sum), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_sum_f64(data, weights, 3, NULL), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_sum_f64(data, weights, 0, &sum), FC_ERR_INVALID_ARG);
}

TEST(test_weighted_sum_large_array) {
    const size_t n = 10000;
    double data[n];
    double weights[n];
    for (size_t i = 0; i < n; i++) {
        data[i] = (double) i;
        weights[i] = 1.0;
    }

    double sum;
    fc_status_t status = fc_stats_weighted_sum_f64(data, weights, n, &sum);

    ASSERT_EQ(status, FC_OK);
    double expected = (double) n * (n - 1) / 2.0;
    FC_TEST_ASSERT_DOUBLE_EQ(sum, expected, expected * 1e-10);
}

TEST(test_weighted_mean_basic) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    double mean;

    fc_status_t status = fc_stats_weighted_mean_f64(data, weights, 3, &mean);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 2.25, TEST_TOLERANCE);
}

TEST(test_weighted_variance_basic) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    double variance;

    fc_status_t status = fc_stats_weighted_variance_f64(data, weights, 3, &variance);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.6875, TEST_TOLERANCE);
}

TEST(test_weighted_mean_variance_combined) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    double mean;
    double variance;

    fc_status_t status = fc_stats_weighted_mean_variance_f64(data, weights, 3, &mean, &variance);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 2.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.6875, TEST_TOLERANCE);
}

TEST(test_weighted_stddev_basic) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    double stddev;

    fc_status_t status = fc_stats_weighted_stddev_f64(data, weights, 3, &stddev);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev, sqrt(0.6875), TEST_TOLERANCE);
}

TEST(test_weighted_zero_weights_allowed) {
    double data[] = {10.0, 20.0, 30.0};
    double weights[] = {0.0, 1.0, 0.0};
    double mean;
    double variance;

    fc_status_t status = fc_stats_weighted_mean_variance_f64(data, weights, 3, &mean, &variance);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 20.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.0, TEST_TOLERANCE);
}

TEST(test_weighted_all_zero_weights) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {0.0, 0.0, 0.0};
    double mean;

    fc_status_t status = fc_stats_weighted_mean_f64(data, weights, 3, &mean);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_negative_weight) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, -1.0, 2.0};
    double mean;

    fc_status_t status = fc_stats_weighted_mean_f64(data, weights, 3, &mean);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_null_inputs) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 1.0};
    double mean;

    ASSERT_EQ(fc_stats_weighted_mean_f64(NULL, weights, 3, &mean), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_mean_f64(data, NULL, 3, &mean), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_mean_f64(data, weights, 3, NULL), FC_ERR_INVALID_ARG);
}

TEST(test_weighted_zero_length) {
    double data[] = {1.0};
    double weights[] = {1.0};
    double mean;

    fc_status_t status = fc_stats_weighted_mean_f64(data, weights, 0, &mean);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_nan_data) {
    double data[] = {1.0, NAN, 3.0};
    double weights[] = {1.0, 1.0, 1.0};
    double mean;

    fc_status_t status = fc_stats_weighted_mean_f64(data, weights, 3, &mean);

    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_weighted_nan_weight) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, NAN, 1.0};
    double mean;

    fc_status_t status = fc_stats_weighted_mean_f64(data, weights, 3, &mean);

    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_weighted_sum_batch) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double weights[] = {1.0, 1.0, 2.0, 2.0, 3.0, 1.0};
    double sums[2];

    fc_status_t status = fc_stats_weighted_sum_batch_f64(sums, data, weights, 2, 3);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(sums[0], 9.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(sums[1], 29.0, TEST_TOLERANCE);
}

TEST(test_weighted_sum_batch_null_inputs) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 1.0};
    double sums[1];

    ASSERT_EQ(fc_stats_weighted_sum_batch_f64(NULL, data, weights, 1, 3), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_sum_batch_f64(sums, NULL, weights, 1, 3), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_sum_batch_f64(sums, data, NULL, 1, 3), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_sum_batch_f64(sums, data, weights, 0, 3), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_sum_batch_f64(sums, data, weights, 1, 0), FC_ERR_INVALID_ARG);
}

TEST(test_weighted_batch_mean) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    double means[2];

    fc_status_t status = fc_stats_weighted_mean_batch_f64(means, data, weights, 2, 3);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(means[0], 2.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(means[1], 20.0, TEST_TOLERANCE);
}

TEST(test_weighted_batch_variance) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    double variances[2];

    fc_status_t status = fc_stats_weighted_variance_batch_f64(variances, data, weights, 2, 3);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[0], 0.6875, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[1], 50.0, TEST_TOLERANCE);
}

TEST(test_weighted_batch_mean_variance) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    double means[2];
    double variances[2];

    fc_status_t status = fc_stats_weighted_mean_variance_batch_f64(means, variances, data, weights, 2, 3);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(means[0], 2.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(means[1], 20.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[0], 0.6875, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variances[1], 50.0, TEST_TOLERANCE);
}

TEST(test_weighted_batch_stddev) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    double stddevs[2];

    fc_status_t status = fc_stats_weighted_stddev_batch_f64(stddevs, data, weights, 2, 3);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddevs[0], sqrt(0.6875), TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(stddevs[1], sqrt(50.0), TEST_TOLERANCE);
}

TEST(test_weighted_batch_invalid_zero_weight_group) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    double means[2];

    fc_status_t status = fc_stats_weighted_mean_batch_f64(means, data, weights, 2, 3);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_batch_nan_group) {
    double data[] = {1.0, 2.0, 3.0, 10.0, NAN, 30.0};
    double weights[] = {1.0, 1.0, 1.0, 1.0, 2.0, 1.0};
    double means[2];

    fc_status_t status = fc_stats_weighted_mean_batch_f64(means, data, weights, 2, 3);

    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_weighted_batch_dimension_overflow) {
    double data[] = {1.0};
    double weights[] = {1.0};
    double means[1];

    fc_status_t status = fc_stats_weighted_mean_batch_f64(means, data, weights, SIZE_MAX, 2);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_bigfloat_mean_variance) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    fc_bigfloat_t* mean = NULL;
    fc_bigfloat_t* variance = NULL;
    double mean_f64 = 0.0;
    double variance_f64 = 0.0;

    ASSERT_EQ(fc_bigfloat_create(&mean), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&variance), FC_OK);

    fc_status_t status = fc_stats_weighted_mean_variance_bigfloat_f64(data, weights, 3, mean, variance, 256);

    ASSERT_EQ(status, FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(mean, &mean_f64), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(variance, &variance_f64), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean_f64, 2.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variance_f64, 0.6875, TEST_TOLERANCE);

    fc_bigfloat_destroy(mean);
    fc_bigfloat_destroy(variance);
}

TEST(test_weighted_bigfloat_stddev) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};
    fc_bigfloat_t* stddev = NULL;
    double stddev_f64 = 0.0;

    ASSERT_EQ(fc_bigfloat_create(&stddev), FC_OK);

    fc_status_t status = fc_stats_weighted_stddev_bigfloat_f64(data, weights, 3, stddev, 256);

    ASSERT_EQ(status, FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(stddev, &stddev_f64), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev_f64, sqrt(0.6875), TEST_TOLERANCE);

    fc_bigfloat_destroy(stddev);
}

TEST(test_weighted_bigfloat_sum) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 2.0};

    fc_bigfloat_t* sum = NULL;
    ASSERT_EQ(fc_bigfloat_create(&sum), FC_OK);

    fc_status_t status = fc_stats_weighted_sum_bigfloat_f64(data, weights, 3, sum, 256);
    ASSERT_EQ(status, FC_OK);

    double result = 0.0;
    ASSERT_EQ(fc_bigfloat_get_f64(sum, &result), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 9.0, TEST_TOLERANCE);

    fc_bigfloat_destroy(sum);
}

TEST(test_weighted_bigfloat_batch_sum) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double weights[] = {1.0, 1.0, 2.0, 2.0, 3.0, 1.0};

    fc_bigfloat_t* sums[2];
    ASSERT_EQ(fc_bigfloat_create(&sums[0]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&sums[1]), FC_OK);

    fc_status_t status = fc_stats_weighted_sum_batch_bigfloat_f64(sums, data, weights, 2, 3, 256);
    ASSERT_EQ(status, FC_OK);

    double result0 = 0.0, result1 = 0.0;
    ASSERT_EQ(fc_bigfloat_get_f64(sums[0], &result0), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(sums[1], &result1), FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(result0, 9.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(result1, 29.0, TEST_TOLERANCE);

    fc_bigfloat_destroy(sums[0]);
    fc_bigfloat_destroy(sums[1]);
}

TEST(test_weighted_bigfloat_batch_mean) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    fc_bigfloat_t* means[2] = {NULL, NULL};
    double mean0 = 0.0;
    double mean1 = 0.0;

    ASSERT_EQ(fc_bigfloat_create(&means[0]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&means[1]), FC_OK);

    fc_status_t status = fc_stats_weighted_mean_batch_bigfloat_f64(means, data, weights, 2, 3, 256);

    ASSERT_EQ(status, FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(means[0], &mean0), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(means[1], &mean1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean0, 2.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(mean1, 20.0, TEST_TOLERANCE);

    fc_bigfloat_destroy(means[0]);
    fc_bigfloat_destroy(means[1]);
}

TEST(test_weighted_bigfloat_batch_variance) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    fc_bigfloat_t* variances[2] = {NULL, NULL};
    double variance0 = 0.0;
    double variance1 = 0.0;

    ASSERT_EQ(fc_bigfloat_create(&variances[0]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&variances[1]), FC_OK);

    fc_status_t status = fc_stats_weighted_variance_batch_bigfloat_f64(variances, data, weights, 2, 3, 256);

    ASSERT_EQ(status, FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(variances[0], &variance0), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(variances[1], &variance1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance0, 0.6875, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variance1, 50.0, TEST_TOLERANCE);

    fc_bigfloat_destroy(variances[0]);
    fc_bigfloat_destroy(variances[1]);
}

TEST(test_weighted_bigfloat_batch_mean_variance) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    fc_bigfloat_t* means[2] = {NULL, NULL};
    fc_bigfloat_t* variances[2] = {NULL, NULL};
    double mean0 = 0.0;
    double mean1 = 0.0;
    double variance0 = 0.0;
    double variance1 = 0.0;

    ASSERT_EQ(fc_bigfloat_create(&means[0]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&means[1]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&variances[0]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&variances[1]), FC_OK);

    fc_status_t status = fc_stats_weighted_mean_variance_batch_bigfloat_f64(means, variances, data, weights, 2, 3, 256);

    ASSERT_EQ(status, FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(means[0], &mean0), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(means[1], &mean1), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(variances[0], &variance0), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(variances[1], &variance1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean0, 2.25, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(mean1, 20.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variance0, 0.6875, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(variance1, 50.0, TEST_TOLERANCE);

    fc_bigfloat_destroy(means[0]);
    fc_bigfloat_destroy(means[1]);
    fc_bigfloat_destroy(variances[0]);
    fc_bigfloat_destroy(variances[1]);
}

TEST(test_weighted_bigfloat_batch_stddev) {
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    fc_bigfloat_t* stddevs[2] = {NULL, NULL};
    double stddev0 = 0.0;
    double stddev1 = 0.0;

    ASSERT_EQ(fc_bigfloat_create(&stddevs[0]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&stddevs[1]), FC_OK);

    fc_status_t status = fc_stats_weighted_stddev_batch_bigfloat_f64(stddevs, data, weights, 2, 3, 256);

    ASSERT_EQ(status, FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(stddevs[0], &stddev0), FC_OK);
    ASSERT_EQ(fc_bigfloat_get_f64(stddevs[1], &stddev1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev0, sqrt(0.6875), TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev1, sqrt(50.0), TEST_TOLERANCE);

    fc_bigfloat_destroy(stddevs[0]);
    fc_bigfloat_destroy(stddevs[1]);
}

TEST(test_weighted_bigfloat_batch_validation) {
    double nan_data[] = {1.0, INFINITY, 3.0, 10.0, NAN, 30.0};
    double inf_data[] = {1.0, INFINITY, 3.0, 10.0, 20.0, 30.0};
    double data[] = {1.0, 2.0, 3.0, 10.0, 20.0, 30.0};
    double weights[] = {1.0, 1.0, 2.0, 1.0, 2.0, 1.0};
    double negative_weights[] = {1.0, 1.0, 2.0, 1.0, -2.0, 1.0};
    fc_bigfloat_t* means[2] = {NULL, NULL};

    ASSERT_EQ(fc_bigfloat_create(&means[0]), FC_OK);
    ASSERT_EQ(fc_bigfloat_create(&means[1]), FC_OK);

    ASSERT_EQ(fc_stats_weighted_mean_batch_bigfloat_f64(means, nan_data, weights, 2, 3, 256), FC_ERR_NAN_INPUT);
    ASSERT_EQ(fc_stats_weighted_mean_batch_bigfloat_f64(means, inf_data, weights, 2, 3, 256), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_mean_batch_bigfloat_f64(means, data, negative_weights, 2, 3, 256), FC_ERR_INVALID_ARG);

    fc_bigfloat_destroy(means[1]);
    means[1] = NULL;
    ASSERT_EQ(fc_stats_weighted_mean_batch_bigfloat_f64(means, nan_data, weights, 2, 3, 256), FC_ERR_INVALID_ARG);

    fc_bigfloat_destroy(means[0]);
}

TEST(test_weighted_bigfloat_invalid_inputs) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, -1.0, 2.0};
    fc_bigfloat_t* mean = NULL;

    ASSERT_EQ(fc_bigfloat_create(&mean), FC_OK);

    ASSERT_EQ(fc_stats_weighted_mean_bigfloat_f64(data, weights, 3, mean, 256), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_mean_bigfloat_f64(data, weights, 3, NULL, 256), FC_ERR_INVALID_ARG);

    fc_bigfloat_destroy(mean);
}

TEST(test_weighted_median_basic) {
    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double weights[] = {1.0, 1.0, 1.0, 1.0, 1.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0, TEST_TOLERANCE);
}

TEST(test_weighted_median_skewed) {
    double values[] = {100.0, 200.0, 300.0};
    double weights[] = {10.0, 1.0, 1.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 3, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 100.0, TEST_TOLERANCE);
}

TEST(test_weighted_median_two_elements) {
    double values[] = {10.0, 20.0};
    double weights[] = {3.0, 1.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 2, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 10.0, TEST_TOLERANCE);
}

TEST(test_weighted_median_single_element) {
    double values[] = {42.0};
    double weights[] = {1.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 1, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 42.0, TEST_TOLERANCE);
}

TEST(test_weighted_median_unsorted) {
    double values[] = {5.0, 1.0, 3.0, 4.0, 2.0};
    double weights[] = {1.0, 1.0, 1.0, 1.0, 1.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0, TEST_TOLERANCE);
}

TEST(test_weighted_median_zero_weights) {
    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double weights[] = {0.0, 0.0, 1.0, 0.0, 0.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 5, &result);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(result, 3.0, TEST_TOLERANCE);
}

TEST(test_weighted_median_error_zero_size) {
    double values[] = {1.0};
    double weights[] = {1.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 0, &result);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_median_error_null_inputs) {
    double values[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 1.0};
    double result;

    ASSERT_EQ(fc_stats_weighted_median_f64(NULL, weights, 3, &result), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_median_f64(values, NULL, 3, &result), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_median_f64(values, weights, 3, NULL), FC_ERR_INVALID_ARG);
}

TEST(test_weighted_median_error_all_weights_zero) {
    double values[] = {1.0, 2.0, 3.0};
    double weights[] = {0.0, 0.0, 0.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 3, &result);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_median_error_negative_weight) {
    double values[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, -1.0, 1.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(values, weights, 3, &result);

    ASSERT_EQ(status, FC_ERR_NAN_INPUT);
}

TEST(test_weighted_median_crypto_scenario) {
    double exchange_prices[] = {50000.0, 50050.0, 49900.0, 50020.0, 51000.0};
    double volumes[] = {100.0, 150.0, 120.0, 130.0, 10.0};
    double result;

    fc_status_t status = fc_stats_weighted_median_f64(exchange_prices, volumes, 5, &result);

    ASSERT_EQ(status, FC_OK);
    ASSERT_TRUE(result >= 49000.0 && result <= 52000.0);
}

TEST(test_weighted_median_batch_basic) {
    double values[] = {1.0, 2.0, 3.0, 10.0, 20.0};
    double weights[] = {1.0, 1.0, 1.0, 3.0, 1.0};
    size_t offsets[] = {0, 3};
    size_t sizes[] = {3, 2};
    double results[2];

    fc_status_t status = fc_stats_weighted_median_batch_f64(values, weights, offsets, sizes, 2, results);

    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(results[0], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(results[1], 10.0, TEST_TOLERANCE);
}

TEST(test_weighted_median_batch_error_zero_size) {
    double values[] = {1.0, 2.0};
    double weights[] = {1.0, 1.0};
    size_t offsets[] = {0};
    size_t sizes[] = {0};
    double results[1];

    fc_status_t status = fc_stats_weighted_median_batch_f64(values, weights, offsets, sizes, 1, results);

    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_weighted_median_batch_error_null_inputs) {
    double values[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, 1.0, 1.0};
    size_t offsets[] = {0};
    size_t sizes[] = {3};
    double results[1];

    ASSERT_EQ(fc_stats_weighted_median_batch_f64(NULL, weights, offsets, sizes, 1, results), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_median_batch_f64(values, NULL, offsets, sizes, 1, results), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_median_batch_f64(values, weights, NULL, sizes, 1, results), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_median_batch_f64(values, weights, offsets, NULL, 1, results), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_median_batch_f64(values, weights, offsets, sizes, 1, NULL), FC_ERR_INVALID_ARG);
}

void register_weighted_tests(void) {
    RUN_TEST(test_weighted_sum_basic);
    RUN_TEST(test_weighted_sum_zero_weights);
    RUN_TEST(test_weighted_sum_all_zero_weights);
    RUN_TEST(test_weighted_sum_negative_weight);
    RUN_TEST(test_weighted_sum_nan_data);
    RUN_TEST(test_weighted_sum_nan_weight);
    RUN_TEST(test_weighted_sum_null_inputs);
    RUN_TEST(test_weighted_sum_large_array);
    RUN_TEST(test_weighted_mean_basic);
    RUN_TEST(test_weighted_variance_basic);
    RUN_TEST(test_weighted_mean_variance_combined);
    RUN_TEST(test_weighted_stddev_basic);
    RUN_TEST(test_weighted_zero_weights_allowed);
    RUN_TEST(test_weighted_all_zero_weights);
    RUN_TEST(test_weighted_negative_weight);
    RUN_TEST(test_weighted_null_inputs);
    RUN_TEST(test_weighted_zero_length);
    RUN_TEST(test_weighted_nan_data);
    RUN_TEST(test_weighted_nan_weight);
    RUN_TEST(test_weighted_sum_batch);
    RUN_TEST(test_weighted_sum_batch_null_inputs);
    RUN_TEST(test_weighted_batch_mean);
    RUN_TEST(test_weighted_batch_variance);
    RUN_TEST(test_weighted_batch_mean_variance);
    RUN_TEST(test_weighted_batch_stddev);
    RUN_TEST(test_weighted_batch_invalid_zero_weight_group);
    RUN_TEST(test_weighted_batch_nan_group);
    RUN_TEST(test_weighted_batch_dimension_overflow);
    RUN_TEST(test_weighted_bigfloat_mean_variance);
    RUN_TEST(test_weighted_bigfloat_stddev);
    RUN_TEST(test_weighted_bigfloat_sum);
    RUN_TEST(test_weighted_bigfloat_batch_sum);
    RUN_TEST(test_weighted_bigfloat_batch_mean);
    RUN_TEST(test_weighted_bigfloat_batch_variance);
    RUN_TEST(test_weighted_bigfloat_batch_mean_variance);
    RUN_TEST(test_weighted_bigfloat_batch_stddev);
    RUN_TEST(test_weighted_bigfloat_batch_validation);
    RUN_TEST(test_weighted_bigfloat_invalid_inputs);
    RUN_TEST(test_weighted_median_basic);
    RUN_TEST(test_weighted_median_skewed);
    RUN_TEST(test_weighted_median_two_elements);
    RUN_TEST(test_weighted_median_single_element);
    RUN_TEST(test_weighted_median_unsorted);
    RUN_TEST(test_weighted_median_zero_weights);
    RUN_TEST(test_weighted_median_error_zero_size);
    RUN_TEST(test_weighted_median_error_null_inputs);
    RUN_TEST(test_weighted_median_error_all_weights_zero);
    RUN_TEST(test_weighted_median_error_negative_weight);
    RUN_TEST(test_weighted_median_crypto_scenario);
    RUN_TEST(test_weighted_median_batch_basic);
    RUN_TEST(test_weighted_median_batch_error_zero_size);
    RUN_TEST(test_weighted_median_batch_error_null_inputs);
}
