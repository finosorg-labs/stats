/**
 * @file test_ema.c
 * @brief Unit tests for EMA computation
 */

#include "test_framework.h"
#include "../include/ema.h"
#include <math.h>
#include <stdlib.h>

#define TEST_TOLERANCE 1e-10

/**
 * @brief Helper function to compute expected EMA
 */
static void compute_expected_ema(const double* data, size_t n, double alpha, double init, double* expected) {
    double one_minus_alpha = 1.0 - alpha;
    double current_ema     = init;

    for (size_t i = 0; i < n; i++) {
        current_ema = alpha * data[i] + one_minus_alpha * current_ema;
        expected[i] = current_ema;
    }
}

TEST(test_ema_basic) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double ema[5];
    double expected[5];
    double alpha = 0.5;

    fc_status_t status = fc_stats_ema_f64(data, 5, alpha, ema);
    ASSERT_EQ(status, FC_OK);

    compute_expected_ema(data, 5, alpha, data[0], expected);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ema[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_ema_custom_init) {
    double data[]  = {10.0, 20.0, 30.0, 40.0, 50.0};
    double ema[5];
    double expected[5];
    double alpha      = 0.3;
    double init_value = 5.0;

    fc_status_t status = fc_stats_ema_f64_init(data, 5, alpha, init_value, ema);
    ASSERT_EQ(status, FC_OK);

    compute_expected_ema(data, 5, alpha, init_value, expected);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ema[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_ema_alpha_one) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double ema[5];
    double alpha = 1.0;

    fc_status_t status = fc_stats_ema_f64(data, 5, alpha, ema);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ema[i], data[i], TEST_TOLERANCE);
    }
}

TEST(test_ema_small_alpha) {
    double data[] = {100.0, 100.0, 100.0, 100.0, 100.0};
    double ema[5];
    double alpha = 0.1;

    fc_status_t status = fc_stats_ema_f64(data, 5, alpha, ema);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(ema[4], 100.0, TEST_TOLERANCE);
}

TEST(test_ema_financial_alpha) {
    double data[] = {100.0, 102.0, 101.0, 103.0, 105.0, 104.0, 106.0, 108.0};
    double ema[8];
    int period    = 5;
    double alpha  = 2.0 / (period + 1);

    fc_status_t status = fc_stats_ema_f64(data, 8, alpha, ema);
    ASSERT_EQ(status, FC_OK);

    double expected[8];
    compute_expected_ema(data, 8, alpha, data[0], expected);

    for (size_t i = 0; i < 8; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ema[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_ema_inplace) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double expected[5];
    double alpha = 0.5;

    compute_expected_ema(data, 5, alpha, data[0], expected);

    fc_status_t status = fc_stats_ema_f64(data, 5, alpha, data);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(data[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_ema_batch) {
    double data[] = {
        1.0, 2.0, 3.0, 4.0, 5.0,
        10.0, 20.0, 30.0, 40.0, 50.0,
        100.0, 90.0, 80.0, 70.0, 60.0
    };
    double ema[15];
    double alpha = 0.4;

    fc_status_t status = fc_stats_ema_batch_f64(data, 3, 5, alpha, ema);
    ASSERT_EQ(status, FC_OK);

    for (size_t series = 0; series < 3; series++) {
        double expected[5];
        compute_expected_ema(&data[series * 5], 5, alpha, data[series * 5], expected);

        for (size_t i = 0; i < 5; i++) {
            FC_TEST_ASSERT_DOUBLE_EQ(ema[series * 5 + i], expected[i], TEST_TOLERANCE);
        }
    }
}

TEST(test_ema_large) {
    const size_t n = 10000;
    double* data   = malloc(n * sizeof(double));
    double* ema    = malloc(n * sizeof(double));
    double alpha   = 0.2;

    ASSERT_NOT_NULL(data);
    ASSERT_NOT_NULL(ema);

    for (size_t i = 0; i < n; i++) {
        data[i] = sin((double) i * 0.01) * 100.0 + 100.0;
    }

    fc_status_t status = fc_stats_ema_f64(data, n, alpha, ema);
    ASSERT_EQ(status, FC_OK);

    double expected[10];
    compute_expected_ema(data, 10, alpha, data[0], expected);

    for (size_t i = 0; i < 10; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ema[i], expected[i], TEST_TOLERANCE);
    }

    free(data);
    free(ema);
}

TEST(test_ema_null_pointer) {
    double data[] = {1.0, 2.0, 3.0};
    double ema[3];

    ASSERT_EQ(fc_stats_ema_f64(NULL, 3, 0.5, ema), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_ema_f64(data, 3, 0.5, NULL), FC_ERR_INVALID_ARG);
}

TEST(test_ema_invalid_size) {
    double data[] = {1.0, 2.0, 3.0};
    double ema[3];

    ASSERT_EQ(fc_stats_ema_f64(data, 0, 0.5, ema), FC_ERR_INVALID_ARG);
}

TEST(test_ema_invalid_alpha) {
    double data[] = {1.0, 2.0, 3.0};
    double ema[3];

    ASSERT_EQ(fc_stats_ema_f64(data, 3, 0.0, ema), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_ema_f64(data, 3, -0.1, ema), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_stats_ema_f64(data, 3, 1.5, ema), FC_ERR_INVALID_ARG);
}

TEST(test_ema_single_element) {
    double data[] = {42.0};
    double ema[1];
    double alpha = 0.5;

    fc_status_t status = fc_stats_ema_f64(data, 1, alpha, ema);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(ema[0], 42.0, TEST_TOLERANCE);
}

TEST(test_ema_negative_values) {
    double data[] = {-10.0, -5.0, 0.0, 5.0, 10.0};
    double ema[5];
    double expected[5];
    double alpha = 0.3;

    fc_status_t status = fc_stats_ema_f64(data, 5, alpha, ema);
    ASSERT_EQ(status, FC_OK);

    compute_expected_ema(data, 5, alpha, data[0], expected);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ema[i], expected[i], TEST_TOLERANCE);
    }
}

void register_ema_tests(void) {
    RUN_TEST(test_ema_basic);
    RUN_TEST(test_ema_custom_init);
    RUN_TEST(test_ema_alpha_one);
    RUN_TEST(test_ema_small_alpha);
    RUN_TEST(test_ema_financial_alpha);
    RUN_TEST(test_ema_inplace);
    RUN_TEST(test_ema_batch);
    RUN_TEST(test_ema_large);
    RUN_TEST(test_ema_null_pointer);
    RUN_TEST(test_ema_invalid_size);
    RUN_TEST(test_ema_invalid_alpha);
    RUN_TEST(test_ema_single_element);
    RUN_TEST(test_ema_negative_values);
}
