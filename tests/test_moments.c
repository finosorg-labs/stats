/**
 * @file test_moments.c
 * @brief Unit tests for higher-order moments
 */

#include "test_framework.h"
#include "../include/moments.h"
#include <math.h>
#include <float.h>

/* Test tolerance for floating-point comparisons */
#define TEST_TOLERANCE 1e-10

/*
 * Test: Moments initialization
 */

TEST(test_moments_init) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    ASSERT_EQ(state.n, 0);
    FC_TEST_ASSERT_DOUBLE_EQ(state.mean, 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(state.m2, 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(state.m3, 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(state.m4, 0.0, TEST_TOLERANCE);
}

/*
 * Test: Single value updates
 */

TEST(test_moments_update_single) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    fc_stats_moments_update(&state, 1.0);
    ASSERT_EQ(state.n, 1);
    FC_TEST_ASSERT_DOUBLE_EQ(state.mean, 1.0, TEST_TOLERANCE);

    fc_stats_moments_update(&state, 2.0);
    ASSERT_EQ(state.n, 2);
    FC_TEST_ASSERT_DOUBLE_EQ(state.mean, 1.5, TEST_TOLERANCE);

    fc_stats_moments_update(&state, 3.0);
    ASSERT_EQ(state.n, 3);
    FC_TEST_ASSERT_DOUBLE_EQ(state.mean, 2.0, TEST_TOLERANCE);
}

/*
 * Test: Skewness - symmetric distribution
 */

TEST(test_moments_skewness_symmetric) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    double symmetric_data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    fc_stats_moments_update_batch(&state, symmetric_data, 5);

    double skew = fc_stats_moments_skewness(&state);
    FC_TEST_ASSERT_DOUBLE_EQ(skew, 0.0, 0.1);
}

/*
 * Test: Skewness - right-skewed distribution
 */

TEST(test_moments_skewness_right_skewed) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    double right_skewed[] = {1.0, 1.0, 1.0, 2.0, 10.0};
    fc_stats_moments_update_batch(&state, right_skewed, 5);

    double skew = fc_stats_moments_skewness(&state);
    ASSERT_TRUE(skew > 0.5);
}

/*
 * Test: Skewness - left-skewed distribution
 */

TEST(test_moments_skewness_left_skewed) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    double left_skewed[] = {1.0, 9.0, 10.0, 10.0, 10.0};
    fc_stats_moments_update_batch(&state, left_skewed, 5);

    double skew = fc_stats_moments_skewness(&state);
    ASSERT_TRUE(skew < -0.5);
}

/*
 * Test: Kurtosis computation
 */

TEST(test_moments_kurtosis_normal) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    double normal_like[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    fc_stats_moments_update_batch(&state, normal_like, 10);

    double kurt = fc_stats_moments_kurtosis(&state);
    double excess_kurt = fc_stats_moments_excess_kurtosis(&state);

    ASSERT_TRUE(isfinite(kurt));
    ASSERT_TRUE(isfinite(excess_kurt));
    FC_TEST_ASSERT_DOUBLE_EQ(excess_kurt, kurt - 3.0, TEST_TOLERANCE);
}

/*
 * Test: Insufficient data handling
 */

TEST(test_moments_insufficient_data) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    double skew = fc_stats_moments_skewness(&state);
    ASSERT_TRUE(isnan(skew));

    fc_stats_moments_update(&state, 1.0);
    skew = fc_stats_moments_skewness(&state);
    ASSERT_TRUE(isnan(skew));

    fc_stats_moments_update(&state, 2.0);
    skew = fc_stats_moments_skewness(&state);
    ASSERT_TRUE(isnan(skew));

    double kurt = fc_stats_moments_kurtosis(&state);
    ASSERT_TRUE(isnan(kurt));

    fc_stats_moments_update(&state, 3.0);
    skew = fc_stats_moments_skewness(&state);
    ASSERT_TRUE(isfinite(skew));

    kurt = fc_stats_moments_kurtosis(&state);
    ASSERT_TRUE(isnan(kurt));

    fc_stats_moments_update(&state, 4.0);
    kurt = fc_stats_moments_kurtosis(&state);
    ASSERT_TRUE(isfinite(kurt));
}

/*
 * Test: Constant data handling
 */

TEST(test_moments_constant_data) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    double constant[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    fc_stats_moments_update_batch(&state, constant, 5);

    double skew = fc_stats_moments_skewness(&state);
    double kurt = fc_stats_moments_kurtosis(&state);

    ASSERT_TRUE(isnan(skew));
    ASSERT_TRUE(isnan(kurt));
}

/*
 * Test: Batch skewness computation
 */

TEST(test_batch_skewness) {
    double data[] = {
        1.0, 2.0, 3.0, 4.0, 5.0,
        1.0, 1.0, 1.0, 2.0, 10.0,
        1.0, 9.0, 10.0, 10.0, 10.0
    };
    double out[3];

    fc_status_t status = fc_stats_skewness_f64(out, data, 3, 5);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(out[0], 0.0, 0.1);
    ASSERT_TRUE(out[1] > 0.5);
    ASSERT_TRUE(out[2] < -0.5);
}

/*
 * Test: Batch kurtosis computation
 */

TEST(test_batch_kurtosis) {
    double data[] = {
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
        1.0, 1.0, 1.0, 10.0, 10.0, 10.0
    };
    double out[2];

    fc_status_t status = fc_stats_kurtosis_f64(out, data, 2, 6);
    ASSERT_EQ(status, FC_OK);

    ASSERT_TRUE(isfinite(out[0]));
    ASSERT_TRUE(isfinite(out[1]));
}

/*
 * Test: Batch excess kurtosis computation
 */

TEST(test_batch_excess_kurtosis) {
    double data[] = {
        1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
        1.0, 1.0, 1.0, 10.0, 10.0, 10.0
    };
    double out_kurt[2];
    double out_excess[2];

    fc_status_t status1 = fc_stats_kurtosis_f64(out_kurt, data, 2, 6);
    fc_status_t status2 = fc_stats_excess_kurtosis_f64(out_excess, data, 2, 6);

    ASSERT_EQ(status1, FC_OK);
    ASSERT_EQ(status2, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(out_excess[0], out_kurt[0] - 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(out_excess[1], out_kurt[1] - 3.0, TEST_TOLERANCE);
}

/*
 * Test: Invalid arguments handling
 */

TEST(test_batch_invalid_args) {
    double data[10] = {0};
    double out[2];

    fc_status_t status = fc_stats_skewness_f64(NULL, data, 2, 5);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_skewness_f64(out, NULL, 2, 5);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_skewness_f64(out, data, 0, 5);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_skewness_f64(out, data, 2, 2);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stats_kurtosis_f64(out, data, 2, 3);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

/*
 * Test: NaN handling
 */

TEST(test_nan_handling) {
    fc_moments_state_t state;
    fc_stats_moments_init(&state);

    fc_stats_moments_update(&state, 1.0);
    fc_stats_moments_update(&state, NAN);
    fc_stats_moments_update(&state, 2.0);
    fc_stats_moments_update(&state, 3.0);

    ASSERT_EQ(state.n, 3);
    FC_TEST_ASSERT_DOUBLE_EQ(state.mean, 2.0, TEST_TOLERANCE);
}

/*
 * Test suite registration
 */

void register_moments_tests(void) {
    RUN_TEST(test_moments_init);
    RUN_TEST(test_moments_update_single);
    RUN_TEST(test_moments_skewness_symmetric);
    RUN_TEST(test_moments_skewness_right_skewed);
    RUN_TEST(test_moments_skewness_left_skewed);
    RUN_TEST(test_moments_kurtosis_normal);
    RUN_TEST(test_moments_insufficient_data);
    RUN_TEST(test_moments_constant_data);
    RUN_TEST(test_batch_skewness);
    RUN_TEST(test_batch_kurtosis);
    RUN_TEST(test_batch_excess_kurtosis);
    RUN_TEST(test_batch_invalid_args);
    RUN_TEST(test_nan_handling);
}
