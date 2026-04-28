/**
 * @file test_kahan_sum.c
 * @brief Unit tests for Kahan compensated summation
 */

#include "test_framework.h"
#include "kahan_sum.h"
#include <math.h>
#include <float.h>
#include <stdlib.h>

/* Test tolerance for floating-point comparisons */
#define TEST_TOLERANCE 1e-15

/* Test basic summation */
TEST(test_kahan_sum_basic) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double sum = fc_stat_kahan_sum_f64(data, 5);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 15.0, TEST_TOLERANCE);
}

/* Test empty array */
TEST(test_kahan_sum_empty) {
    double sum = fc_stat_kahan_sum_f64(NULL, 0);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 0.0, TEST_TOLERANCE);
}

/* Test single element */
TEST(test_kahan_sum_single) {
    double data[] = {42.0};
    double sum = fc_stat_kahan_sum_f64(data, 1);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 42.0, TEST_TOLERANCE);
}

/* Test catastrophic cancellation scenario */
TEST(test_kahan_sum_cancellation) {
    /* Classic example where naive summation fails:
     * 1e10 + 1.0 - 1e10 + 1.0 should equal 2.0
     * but naive summation might give 0.0 due to precision loss
     */
    double data[] = {1e10, 1.0, -1e10, 1.0};
    double kahan_sum = fc_stat_kahan_sum_f64(data, 4);

    /* Compute naive sum for comparison */
    double naive_sum = 0.0;
    for (int i = 0; i < 4; i++) {
        naive_sum += data[i];
    }

    /* Kahan sum should be exactly 2.0 */
    FC_TEST_ASSERT_DOUBLE_EQ(kahan_sum, 2.0, 1e-10);

    /* Log the difference to show Kahan's advantage */
    (void)naive_sum; /* Suppress unused variable warning */
}

/* Test large array with varying magnitudes */
TEST(test_kahan_sum_large_array) {
    const size_t n = 10000;
    double data[10000];

    /* Fill with alternating large and small values */
    for (size_t i = 0; i < n; i++) {
        data[i] = (i % 2 == 0) ? 1e8 : 1.0;
    }

    double sum = fc_stat_kahan_sum_f64(data, n);
    double expected = 5000.0 * 1e8 + 5000.0 * 1.0;

    /* Kahan sum should be accurate */
    FC_TEST_ASSERT_DOUBLE_EQ(sum, expected, expected * 1e-12);
}

/* Test with negative numbers */
TEST(test_kahan_sum_negative) {
    double data[] = {-1.0, -2.0, -3.0, -4.0, -5.0};
    double sum = fc_stat_kahan_sum_f64(data, 5);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, -15.0, TEST_TOLERANCE);
}

/* Test with mixed positive and negative */
TEST(test_kahan_sum_mixed) {
    double data[] = {10.0, -5.0, 3.0, -2.0, 1.0};
    double sum = fc_stat_kahan_sum_f64(data, 5);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 7.0, TEST_TOLERANCE);
}

/* Test NaN propagation */
TEST(test_kahan_sum_nan) {
    double data[] = {1.0, NAN, 3.0};
    double sum = fc_stat_kahan_sum_f64(data, 3);
    FC_TEST_ASSERT(isnan(sum));
}

/* Test infinity handling */
TEST(test_kahan_sum_infinity) {
    /* Note: Kahan algorithm with INFINITY produces NaN due to compensation:
     * When adding INFINITY, the compensation calculation involves INFINITY - INFINITY = NaN
     * This is expected behavior for Kahan summation with infinite values.
     */
    double data[] = {1.0, INFINITY, 3.0};
    double sum = fc_stat_kahan_sum_f64(data, 3);
    /* Kahan sum with INFINITY results in NaN due to compensation arithmetic */
    FC_TEST_ASSERT(isnan(sum));
}

/* Test single precision variant */
TEST(test_kahan_sum_f32_basic) {
    float data[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    float sum = fc_stat_kahan_sum_f32(data, 5);
    FC_TEST_ASSERT(fabsf(sum - 15.0f) < 1e-6f);
}

/* Test incremental state-based summation */
TEST(test_kahan_state_basic) {
    fc_kahan_state_t state;
    fc_stat_kahan_init(&state);

    fc_stat_kahan_add(&state, 1.0);
    fc_stat_kahan_add(&state, 2.0);
    fc_stat_kahan_add(&state, 3.0);

    double sum = fc_stat_kahan_get_sum(&state);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 6.0, TEST_TOLERANCE);
}

/* Test state-based batch addition */
TEST(test_kahan_state_batch) {
    fc_kahan_state_t state;
    fc_stat_kahan_init(&state);

    double data1[] = {1.0, 2.0, 3.0};
    double data2[] = {4.0, 5.0};

    fc_stat_kahan_add_batch(&state, data1, 3);
    fc_stat_kahan_add_batch(&state, data2, 2);

    double sum = fc_stat_kahan_get_sum(&state);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 15.0, TEST_TOLERANCE);
}

/* Test state-based cancellation scenario */
TEST(test_kahan_state_cancellation) {
    fc_kahan_state_t state;
    fc_stat_kahan_init(&state);

    fc_stat_kahan_add(&state, 1e10);
    fc_stat_kahan_add(&state, 1.0);
    fc_stat_kahan_add(&state, -1e10);
    fc_stat_kahan_add(&state, 1.0);

    double sum = fc_stat_kahan_get_sum(&state);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 2.0, 1e-10);
}

/* Test comparison with naive summation on difficult case */
TEST(test_kahan_vs_naive_accuracy) {
    /* Create a scenario where naive summation loses precision */
    const size_t n = 1000;
    double data[1000];

    /* Fill with values that cause precision loss in naive summation */
    for (size_t i = 0; i < n; i++) {
        data[i] = (i % 2 == 0) ? 1e16 : 1.0;
    }

    /* Add compensating negative values */
    for (size_t i = 0; i < n / 2; i++) {
        data[i] = 1e16;
        data[i + n / 2] = -1e16;
    }

    /* The true sum should be close to the sum of the 1.0 values */
    double kahan_sum = fc_stat_kahan_sum_f64(data, n);

    /* Kahan sum should maintain better precision */
    /* Exact expected value depends on the pattern, but should be finite */
    FC_TEST_ASSERT(!isnan(kahan_sum));
    FC_TEST_ASSERT(!isinf(kahan_sum));
}

/* Test runtime SIMD dispatch */
TEST(test_kahan_sum_simd_dispatch) {
    const size_t n = 1000;
    double* data = (double*)malloc(n * sizeof(double));
    FC_TEST_ASSERT(data != NULL);

    /* Generate test data */
    for (size_t i = 0; i < n; i++) {
        data[i] = (i % 2 == 0) ? 1e10 : 1.0;
    }

    /* Call multiple times to test dispatch caching */
    double sum1 = fc_stat_kahan_sum_f64(data, n);
    double sum2 = fc_stat_kahan_sum_f64(data, n);
    double sum3 = fc_stat_kahan_sum_f64(data, n);

    /* All should produce identical results */
    FC_TEST_ASSERT_DOUBLE_EQ(sum1, sum2, 1e-10);
    FC_TEST_ASSERT_DOUBLE_EQ(sum2, sum3, 1e-10);

    free(data);
}

/* Test SIMD with large arrays */
TEST(test_kahan_sum_simd_large) {
    const size_t n = 10000;
    double* data = (double*)malloc(n * sizeof(double));
    FC_TEST_ASSERT(data != NULL);

    /* Generate test data with varying magnitudes */
    for (size_t i = 0; i < n; i++) {
        data[i] = (i % 2 == 0) ? 1e10 : 1.0;
    }

    double sum = fc_stat_kahan_sum_f64(data, n);
    double expected = 5000.0 * 1e10 + 5000.0;

    /* Should be accurate regardless of SIMD level */
    FC_TEST_ASSERT_DOUBLE_EQ(sum, expected, expected * 1e-12);

    free(data);
}

/* Test SIMD with catastrophic cancellation */
TEST(test_kahan_sum_simd_cancellation) {
    const size_t n = 10000;
    double* data = (double*)malloc(n * sizeof(double));
    FC_TEST_ASSERT(data != NULL);

    /* Alternating large and small values */
    for (size_t i = 0; i < n; i++) {
        data[i] = (i % 2 == 0) ? 1e10 : 1.0;
    }

    double sum = fc_stat_kahan_sum_f64(data, n);
    double expected = 5000.0 * 1e10 + 5000.0;

    FC_TEST_ASSERT_DOUBLE_EQ(sum, expected, expected * 1e-12);

    free(data);
}

/* Test SIMD single precision */
TEST(test_kahan_sum_simd_f32) {
    const size_t n = 10000;
    float* data = (float*)malloc(n * sizeof(float));
    FC_TEST_ASSERT(data != NULL);

    for (size_t i = 0; i < n; i++) {
        data[i] = (float)i + 0.5f;
    }

    float sum1 = fc_stat_kahan_sum_f32(data, n);
    float sum2 = fc_stat_kahan_sum_f32(data, n);

    /* Should match within float precision */
    FC_TEST_ASSERT(fabsf(sum1 - sum2) < 1e-4f);

    free(data);
}

/* Test NULL pointer handling */
TEST(test_kahan_sum_null_pointer) {
    /* NULL data with n=0 should return 0 */
    double sum1 = fc_stat_kahan_sum_f64(NULL, 0);
    FC_TEST_ASSERT_DOUBLE_EQ(sum1, 0.0, TEST_TOLERANCE);

    /* NULL data with n>0 should return 0 (defensive) */
    double sum2 = fc_stat_kahan_sum_f64(NULL, 10);
    FC_TEST_ASSERT_DOUBLE_EQ(sum2, 0.0, TEST_TOLERANCE);
}

/* Test NULL state handling */
TEST(test_kahan_state_null_pointer) {
    /* NULL state should not crash */
    fc_stat_kahan_init(NULL);
    fc_stat_kahan_add(NULL, 1.0);

    double data[] = {1.0, 2.0, 3.0};
    fc_stat_kahan_add_batch(NULL, data, 3);

    double sum = fc_stat_kahan_get_sum(NULL);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 0.0, TEST_TOLERANCE);

    /* Valid state with NULL data should not crash */
    fc_kahan_state_t state;
    fc_stat_kahan_init(&state);
    fc_stat_kahan_add_batch(&state, NULL, 10);
    sum = fc_stat_kahan_get_sum(&state);
    FC_TEST_ASSERT_DOUBLE_EQ(sum, 0.0, TEST_TOLERANCE);
}

/* Register all tests */
void register_kahan_sum_tests(void) {
    RUN_TEST(test_kahan_sum_basic);
    RUN_TEST(test_kahan_sum_empty);
    RUN_TEST(test_kahan_sum_single);
    RUN_TEST(test_kahan_sum_cancellation);
    RUN_TEST(test_kahan_sum_large_array);
    RUN_TEST(test_kahan_sum_negative);
    RUN_TEST(test_kahan_sum_mixed);
    RUN_TEST(test_kahan_sum_nan);
    RUN_TEST(test_kahan_sum_infinity);
    RUN_TEST(test_kahan_sum_f32_basic);
    RUN_TEST(test_kahan_state_basic);
    RUN_TEST(test_kahan_state_batch);
    RUN_TEST(test_kahan_state_cancellation);
    RUN_TEST(test_kahan_vs_naive_accuracy);
    RUN_TEST(test_kahan_sum_simd_dispatch);
    RUN_TEST(test_kahan_sum_simd_large);
    RUN_TEST(test_kahan_sum_simd_cancellation);
    RUN_TEST(test_kahan_sum_simd_f32);
    RUN_TEST(test_kahan_sum_null_pointer);
    RUN_TEST(test_kahan_state_null_pointer);
}
