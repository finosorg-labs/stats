/**
 * @file test_weighted.c
 * @brief Unit tests for weighted statistical computations
 */

#include "test_framework.h"
#include "../include/weighted.h"
#include <math.h>
#include <stdint.h>

#define TEST_TOLERANCE 1e-12

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

TEST(test_weighted_bigfloat_invalid_inputs) {
    double data[] = {1.0, 2.0, 3.0};
    double weights[] = {1.0, -1.0, 2.0};
    fc_bigfloat_t* mean = NULL;

    ASSERT_EQ(fc_bigfloat_create(&mean), FC_OK);

    ASSERT_EQ(fc_stats_weighted_mean_bigfloat_f64(data, weights, 3, mean, 256), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_weighted_mean_bigfloat_f64(data, weights, 3, NULL, 256), FC_ERR_INVALID_ARG);

    fc_bigfloat_destroy(mean);
}

void register_weighted_tests(void) {
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
    RUN_TEST(test_weighted_batch_mean);
    RUN_TEST(test_weighted_batch_variance);
    RUN_TEST(test_weighted_batch_mean_variance);
    RUN_TEST(test_weighted_batch_stddev);
    RUN_TEST(test_weighted_batch_invalid_zero_weight_group);
    RUN_TEST(test_weighted_batch_nan_group);
    RUN_TEST(test_weighted_batch_dimension_overflow);
    RUN_TEST(test_weighted_bigfloat_mean_variance);
    RUN_TEST(test_weighted_bigfloat_stddev);
    RUN_TEST(test_weighted_bigfloat_batch_mean);
    RUN_TEST(test_weighted_bigfloat_invalid_inputs);
}
