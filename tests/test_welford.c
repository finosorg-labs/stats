/**
 * @file test_welford.c
 * @brief Unit tests for Welford's online statistics
 */

#include "../include/welford.h"
#include "../../platform/include/simd_detect.h"
#include "test_framework.h"
#include <float.h>
#include <math.h>

#define TEST_TOLERANCE 1e-10

TEST(test_welford_init) {
    fc_welford_state_t state;

    ASSERT_EQ(fc_welford_init(&state), FC_OK);
    ASSERT_EQ(state.count, 0);
    FC_TEST_ASSERT_DOUBLE_EQ(state.mean, 0.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(state.m2, 0.0, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_init(NULL), FC_ERR_INVALID_ARG);
}

TEST(test_welford_update_single) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    ASSERT_EQ(fc_welford_update(&state, 5.0), FC_OK);
    ASSERT_EQ(state.count, 1);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 5.0, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_update(&state, 7.0), FC_OK);
    ASSERT_EQ(state.count, 2);
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 6.0, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_update(NULL, 1.0), FC_ERR_INVALID_ARG);
}

TEST(test_welford_update_nan) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    ASSERT_EQ(fc_welford_update(&state, NAN), FC_ERR_NAN_INPUT);
    ASSERT_EQ(state.count, 0);
}

TEST(test_welford_mean) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 0.0, TEST_TOLERANCE);

    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    for (size_t i = 0; i < 5; i++) {
        fc_welford_update(&state, values[i]);
    }

    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 3.0, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_mean(NULL, &mean), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_mean(&state, NULL), FC_ERR_INVALID_ARG);
}

TEST(test_welford_variance_sample) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.0, TEST_TOLERANCE);

    fc_welford_update(&state, 1.0);
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.0, TEST_TOLERANCE);

    double values[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    fc_welford_init(&state);
    for (size_t i = 0; i < 8; i++) {
        fc_welford_update(&state, values[i]);
    }

    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 4.571428571428571, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_variance(NULL, &variance, 1), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_variance(&state, NULL, 1), FC_ERR_INVALID_ARG);
}

TEST(test_welford_variance_population) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double values[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    for (size_t i = 0; i < 8; i++) {
        fc_welford_update(&state, values[i]);
    }

    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 0), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 4.0, TEST_TOLERANCE);
}

TEST(test_welford_stddev) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double values[] = {2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
    for (size_t i = 0; i < 8; i++) {
        fc_welford_update(&state, values[i]);
    }

    double stddev;
    ASSERT_EQ(fc_welford_stddev(&state, &stddev, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev, 2.138089935299395, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_stddev(&state, &stddev, 0), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev, 2.0, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_stddev(NULL, &stddev, 1), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_stddev(&state, NULL, 1), FC_ERR_INVALID_ARG);
}

TEST(test_welford_count) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    size_t count;
    ASSERT_EQ(fc_welford_count(&state, &count), FC_OK);
    ASSERT_EQ(count, 0);

    fc_welford_update(&state, 1.0);
    fc_welford_update(&state, 2.0);
    fc_welford_update(&state, 3.0);

    ASSERT_EQ(fc_welford_count(&state, &count), FC_OK);
    ASSERT_EQ(count, 3);

    ASSERT_EQ(fc_welford_count(NULL, &count), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_count(&state, NULL), FC_ERR_INVALID_ARG);
}

TEST(test_welford_update_batch) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    ASSERT_EQ(fc_welford_update_batch(&state, values, 5), FC_OK);

    size_t count;
    ASSERT_EQ(fc_welford_count(&state, &count), FC_OK);
    ASSERT_EQ(count, 5);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 3.0, TEST_TOLERANCE);

    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 2.5, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_update_batch(NULL, values, 5), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_update_batch(&state, NULL, 5), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_update_batch(&state, values, 0), FC_ERR_INVALID_ARG);
}

TEST(test_welford_update_batch_nan) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double values[] = {1.0, 2.0, NAN, 4.0, 5.0};
    ASSERT_EQ(fc_welford_update_batch(&state, values, 5), FC_ERR_NAN_INPUT);
}

TEST(test_welford_merge_empty) {
    fc_welford_state_t state_a, state_b;
    fc_welford_init(&state_a);
    fc_welford_init(&state_b);

    ASSERT_EQ(fc_welford_merge(&state_a, &state_b), FC_OK);
    ASSERT_EQ(state_a.count, 0);

    fc_welford_update(&state_a, 5.0);
    ASSERT_EQ(fc_welford_merge(&state_a, &state_b), FC_OK);
    ASSERT_EQ(state_a.count, 1);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state_a, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 5.0, TEST_TOLERANCE);
}

TEST(test_welford_merge_basic) {
    fc_welford_state_t state_a, state_b;
    fc_welford_init(&state_a);
    fc_welford_init(&state_b);

    double values_a[] = {1.0, 2.0, 3.0};
    double values_b[] = {4.0, 5.0, 6.0};

    fc_welford_update_batch(&state_a, values_a, 3);
    fc_welford_update_batch(&state_b, values_b, 3);

    ASSERT_EQ(fc_welford_merge(&state_a, &state_b), FC_OK);

    size_t count;
    ASSERT_EQ(fc_welford_count(&state_a, &count), FC_OK);
    ASSERT_EQ(count, 6);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state_a, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 3.5, TEST_TOLERANCE);

    double variance;
    ASSERT_EQ(fc_welford_variance(&state_a, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 3.5, TEST_TOLERANCE);
}

TEST(test_welford_merge_asymmetric) {
    fc_welford_state_t state_a, state_b;
    fc_welford_init(&state_a);
    fc_welford_init(&state_b);

    double values_a[] = {1.0, 2.0};
    double values_b[] = {3.0, 4.0, 5.0, 6.0};

    fc_welford_update_batch(&state_a, values_a, 2);
    fc_welford_update_batch(&state_b, values_b, 4);

    ASSERT_EQ(fc_welford_merge(&state_a, &state_b), FC_OK);

    size_t count;
    ASSERT_EQ(fc_welford_count(&state_a, &count), FC_OK);
    ASSERT_EQ(count, 6);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state_a, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 3.5, TEST_TOLERANCE);
}

TEST(test_welford_merge_null) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    ASSERT_EQ(fc_welford_merge(NULL, &state), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_merge(&state, NULL), FC_ERR_INVALID_ARG);
}

TEST(test_welford_numerical_stability) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double large_base = 1e9;
    double values[]   = {large_base + 1.0, large_base + 2.0, large_base + 3.0};

    for (size_t i = 0; i < 3; i++) {
        fc_welford_update(&state, values[i]);
    }

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, large_base + 2.0, TEST_TOLERANCE);

    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 1.0, TEST_TOLERANCE);
}

TEST(test_welford_large_dataset) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    const size_t n = 10000;
    for (size_t i = 0; i < n; i++) {
        fc_welford_update(&state, (double) i);
    }

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 4999.5, 1e-6);

    double expected_variance = (n * n - 1.0) / 12.0;
    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, expected_variance, 1.0);
}

TEST(test_welford_reset) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    fc_welford_update(&state, 1.0);
    fc_welford_update(&state, 2.0);
    fc_welford_update(&state, 3.0);

    size_t count;
    ASSERT_EQ(fc_welford_count(&state, &count), FC_OK);
    ASSERT_EQ(count, 3);

    ASSERT_EQ(fc_welford_reset(&state), FC_OK);
    ASSERT_EQ(fc_welford_count(&state, &count), FC_OK);
    ASSERT_EQ(count, 0);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 0.0, TEST_TOLERANCE);
}

TEST(test_welford_update_infinity) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    ASSERT_EQ(fc_welford_update(&state, INFINITY), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_update(&state, -INFINITY), FC_ERR_INVALID_ARG);
    ASSERT_EQ(state.count, 0);
}

TEST(test_welford_update_batch_infinity) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double values[] = {1.0, 2.0, INFINITY, 4.0, 5.0};
    ASSERT_EQ(fc_welford_update_batch(&state, values, 5), FC_ERR_INVALID_ARG);

    double values2[] = {1.0, 2.0, -INFINITY, 4.0, 5.0};
    ASSERT_EQ(fc_welford_update_batch(&state, values2, 5), FC_ERR_INVALID_ARG);
}

TEST(test_welford_batch_small_sizes) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    // Test size 1
    double val1[] = {5.0};
    ASSERT_EQ(fc_welford_update_batch(&state, val1, 1), FC_OK);
    ASSERT_EQ(state.count, 1);

    // Test size 2
    fc_welford_reset(&state);
    double val2[] = {3.0, 7.0};
    ASSERT_EQ(fc_welford_update_batch(&state, val2, 2), FC_OK);
    ASSERT_EQ(state.count, 2);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 5.0, TEST_TOLERANCE);
}

TEST(test_welford_batch_medium_size) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    // Test size 20 (triggers SSE path)
    double values[20];
    for (int i = 0; i < 20; i++) {
        values[i] = (double) (i + 1);
    }

    ASSERT_EQ(fc_welford_update_batch(&state, values, 20), FC_OK);
    ASSERT_EQ(state.count, 20);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 10.5, TEST_TOLERANCE);
}

TEST(test_welford_batch_large_size) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    // Test size 100 (triggers AVX2/AVX512 path)
    double values[100];
    for (int i = 0; i < 100; i++) {
        values[i] = (double) (i + 1);
    }

    ASSERT_EQ(fc_welford_update_batch(&state, values, 100), FC_OK);
    ASSERT_EQ(state.count, 100);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 50.5, TEST_TOLERANCE);

    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 841.6666666666666, 1e-6);
}

TEST(test_welford_merge_into_empty) {
    fc_welford_state_t state_a, state_b;
    fc_welford_init(&state_a);
    fc_welford_init(&state_b);

    double values[] = {1.0, 2.0, 3.0};
    fc_welford_update_batch(&state_b, values, 3);

    ASSERT_EQ(fc_welford_merge(&state_a, &state_b), FC_OK);
    ASSERT_EQ(state_a.count, 3);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state_a, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 2.0, TEST_TOLERANCE);
}

TEST(test_welford_variance_single_value) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    fc_welford_update(&state, 5.0);

    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.0, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_variance(&state, &variance, 0), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.0, TEST_TOLERANCE);
}

TEST(test_welford_stddev_empty) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    double stddev;
    ASSERT_EQ(fc_welford_stddev(&state, &stddev, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(stddev, 0.0, TEST_TOLERANCE);
}

TEST(test_welford_incremental_vs_batch) {
    fc_welford_state_t state_inc, state_batch;
    fc_welford_init(&state_inc);
    fc_welford_init(&state_batch);

    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};

    // Incremental
    for (size_t i = 0; i < 8; i++) {
        fc_welford_update(&state_inc, values[i]);
    }

    // Batch
    fc_welford_update_batch(&state_batch, values, 8);

    // Compare results
    double mean_inc, mean_batch;
    ASSERT_EQ(fc_welford_mean(&state_inc, &mean_inc), FC_OK);
    ASSERT_EQ(fc_welford_mean(&state_batch, &mean_batch), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean_inc, mean_batch, TEST_TOLERANCE);

    double var_inc, var_batch;
    ASSERT_EQ(fc_welford_variance(&state_inc, &var_inc, 1), FC_OK);
    ASSERT_EQ(fc_welford_variance(&state_batch, &var_batch, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(var_inc, var_batch, TEST_TOLERANCE);
}

TEST(test_welford_multiple_merges) {
    fc_welford_state_t states[4];
    for (int i = 0; i < 4; i++) {
        fc_welford_init(&states[i]);
    }

    // Each state gets 2 values
    fc_welford_update(&states[0], 1.0);
    fc_welford_update(&states[0], 2.0);

    fc_welford_update(&states[1], 3.0);
    fc_welford_update(&states[1], 4.0);

    fc_welford_update(&states[2], 5.0);
    fc_welford_update(&states[2], 6.0);

    fc_welford_update(&states[3], 7.0);
    fc_welford_update(&states[3], 8.0);

    // Merge all into states[0]
    fc_welford_merge(&states[0], &states[1]);
    fc_welford_merge(&states[0], &states[2]);
    fc_welford_merge(&states[0], &states[3]);

    ASSERT_EQ(states[0].count, 8);

    double mean;
    ASSERT_EQ(fc_welford_mean(&states[0], &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 4.5, TEST_TOLERANCE);

    double variance;
    ASSERT_EQ(fc_welford_variance(&states[0], &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 6.0, TEST_TOLERANCE);
}

TEST(test_welford_simd_sse_path) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    // Size 16 should trigger SSE path
    double values[16];
    for (int i = 0; i < 16; i++) {
        values[i] = (double) (i + 1);
    }

    ASSERT_EQ(fc_welford_update_batch(&state, values, 16), FC_OK);
    ASSERT_EQ(state.count, 16);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 8.5, TEST_TOLERANCE);
}

TEST(test_welford_simd_avx2_path) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    // Size 32 should trigger AVX2 path
    double values[32];
    for (int i = 0; i < 32; i++) {
        values[i] = (double) (i + 1);
    }

    ASSERT_EQ(fc_welford_update_batch(&state, values, 32), FC_OK);
    ASSERT_EQ(state.count, 32);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 16.5, TEST_TOLERANCE);
}

TEST(test_welford_simd_avx512_path) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    // Size 64 should trigger AVX512 path
    double values[64];
    for (int i = 0; i < 64; i++) {
        values[i] = (double) (i + 1);
    }

    ASSERT_EQ(fc_welford_update_batch(&state, values, 64), FC_OK);
    ASSERT_EQ(state.count, 64);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 32.5, TEST_TOLERANCE);

    double variance;
    ASSERT_EQ(fc_welford_variance(&state, &variance, 1), FC_OK);
    // Sample variance of 1..64: n*(n+1)/12 = 64*65/12 = 346.666...
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 346.6666666666667, 1e-6);
}

TEST(test_welford_simd_large_batch) {
    fc_welford_state_t state;
    fc_welford_init(&state);

    // Size 128 to test chunking logic
    double values[128];
    for (int i = 0; i < 128; i++) {
        values[i] = (double) (i + 1);
    }

    ASSERT_EQ(fc_welford_update_batch(&state, values, 128), FC_OK);
    ASSERT_EQ(state.count, 128);

    double mean;
    ASSERT_EQ(fc_welford_mean(&state, &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 64.5, TEST_TOLERANCE);
}

TEST(test_welford_simd_odd_sizes) {
    fc_welford_state_t state;

    // Test size 17 (16 + 1 remainder)
    fc_welford_init(&state);
    double values17[17];
    for (int i = 0; i < 17; i++) {
        values17[i] = (double) (i + 1);
    }
    ASSERT_EQ(fc_welford_update_batch(&state, values17, 17), FC_OK);
    ASSERT_EQ(state.count, 17);

    // Test size 33 (32 + 1 remainder)
    fc_welford_init(&state);
    double values33[33];
    for (int i = 0; i < 33; i++) {
        values33[i] = (double) (i + 1);
    }
    ASSERT_EQ(fc_welford_update_batch(&state, values33, 33), FC_OK);
    ASSERT_EQ(state.count, 33);

    // Test size 65 (64 + 1 remainder)
    fc_welford_init(&state);
    double values65[65];
    for (int i = 0; i < 65; i++) {
        values65[i] = (double) (i + 1);
    }
    ASSERT_EQ(fc_welford_update_batch(&state, values65, 65), FC_OK);
    ASSERT_EQ(state.count, 65);
}

TEST(test_welford_update_streams_basic) {
    // Test 4 independent streams
    fc_welford_state_t states[4];
    for (int i = 0; i < 4; i++) {
        fc_welford_init(&states[i]);
    }

    // Update each stream with 3 values
    double values1[] = {1.0, 2.0, 3.0, 4.0};
    double values2[] = {5.0, 6.0, 7.0, 8.0};
    double values3[] = {9.0, 10.0, 11.0, 12.0};

    ASSERT_EQ(fc_welford_update_streams(states, values1, 4), FC_OK);
    ASSERT_EQ(fc_welford_update_streams(states, values2, 4), FC_OK);
    ASSERT_EQ(fc_welford_update_streams(states, values3, 4), FC_OK);

    // Verify each stream
    double mean;
    ASSERT_EQ(fc_welford_mean(&states[0], &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 5.0, TEST_TOLERANCE); // (1+5+9)/3

    ASSERT_EQ(fc_welford_mean(&states[1], &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 6.0, TEST_TOLERANCE); // (2+6+10)/3

    ASSERT_EQ(fc_welford_mean(&states[2], &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 7.0, TEST_TOLERANCE); // (3+7+11)/3

    ASSERT_EQ(fc_welford_mean(&states[3], &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 8.0, TEST_TOLERANCE); // (4+8+12)/3
}

TEST(test_welford_update_streams_sse_width) {
    // Test exactly 2 streams (SSE width)
    fc_welford_state_t states[2];
    fc_welford_init(&states[0]);
    fc_welford_init(&states[1]);

    double values[] = {10.0, 20.0};
    ASSERT_EQ(fc_welford_update_streams(states, values, 2), FC_OK);

    double mean;
    ASSERT_EQ(fc_welford_mean(&states[0], &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 10.0, TEST_TOLERANCE);

    ASSERT_EQ(fc_welford_mean(&states[1], &mean), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(mean, 20.0, TEST_TOLERANCE);
}

TEST(test_welford_update_streams_avx2_width) {
    // Test exactly 4 streams (AVX2 width)
    fc_welford_state_t states[4];
    for (int i = 0; i < 4; i++) {
        fc_welford_init(&states[i]);
    }

    double values[] = {1.0, 2.0, 3.0, 4.0};
    ASSERT_EQ(fc_welford_update_streams(states, values, 4), FC_OK);

    for (int i = 0; i < 4; i++) {
        double mean;
        ASSERT_EQ(fc_welford_mean(&states[i], &mean), FC_OK);
        FC_TEST_ASSERT_DOUBLE_EQ(mean, (double)(i + 1), TEST_TOLERANCE);
    }
}

TEST(test_welford_update_streams_avx512_width) {
    // Test exactly 8 streams (AVX-512 width)
    fc_welford_state_t states[8];
    for (int i = 0; i < 8; i++) {
        fc_welford_init(&states[i]);
    }

    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    ASSERT_EQ(fc_welford_update_streams(states, values, 8), FC_OK);

    for (int i = 0; i < 8; i++) {
        double mean;
        ASSERT_EQ(fc_welford_mean(&states[i], &mean), FC_OK);
        FC_TEST_ASSERT_DOUBLE_EQ(mean, (double)(i + 1), TEST_TOLERANCE);
    }
}

TEST(test_welford_update_streams_large) {
    // Test 100 streams (realistic scenario)
    const size_t n_streams = 100;
    fc_welford_state_t states[100];
    for (size_t i = 0; i < n_streams; i++) {
        fc_welford_init(&states[i]);
    }

    // Update 10 times
    for (int iter = 0; iter < 10; iter++) {
        double values[100];
        for (size_t i = 0; i < n_streams; i++) {
            values[i] = (double)(iter * 100 + i);
        }
        ASSERT_EQ(fc_welford_update_streams(states, values, n_streams), FC_OK);
    }

    // Verify counts
    for (size_t i = 0; i < n_streams; i++) {
        size_t count;
        ASSERT_EQ(fc_welford_count(&states[i], &count), FC_OK);
        ASSERT_EQ(count, 10);
    }
}

TEST(test_welford_update_streams_variance) {
    // Test variance calculation with streams
    fc_welford_state_t states[3];
    for (int i = 0; i < 3; i++) {
        fc_welford_init(&states[i]);
    }

    // Stream 0: [2, 4, 4, 4, 5, 5, 7, 9]
    // Stream 1: [1, 2, 3, 4, 5]
    // Stream 2: [10, 10, 10]

    double vals1[] = {2.0, 1.0, 10.0};
    double vals2[] = {4.0, 2.0, 10.0};
    double vals3[] = {4.0, 3.0, 10.0};
    double vals4[] = {4.0, 4.0, 0.0}; // Stream 2 done
    double vals5[] = {5.0, 5.0, 0.0};
    double vals6[] = {5.0, 0.0, 0.0}; // Stream 1 done
    double vals7[] = {7.0, 0.0, 0.0};
    double vals8[] = {9.0, 0.0, 0.0};

    fc_welford_update_streams(states, vals1, 3);
    fc_welford_update_streams(states, vals2, 3);
    fc_welford_update_streams(states, vals3, 3);
    fc_welford_update_streams(&states[0], &vals4[0], 2); // Only first 2 streams
    fc_welford_update_streams(&states[0], &vals5[0], 2);
    fc_welford_update(&states[0], vals6[0]);
    fc_welford_update(&states[0], vals7[0]);
    fc_welford_update(&states[0], vals8[0]);

    // Verify stream 0: [2, 4, 4, 4, 5, 5, 7, 9]
    double variance;
    ASSERT_EQ(fc_welford_variance(&states[0], &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 4.571428571428571, TEST_TOLERANCE);

    // Verify stream 1: [1, 2, 3, 4, 5]
    ASSERT_EQ(fc_welford_variance(&states[1], &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 2.5, TEST_TOLERANCE);

    // Verify stream 2: [10, 10, 10]
    ASSERT_EQ(fc_welford_variance(&states[2], &variance, 1), FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(variance, 0.0, TEST_TOLERANCE);
}

TEST(test_welford_update_streams_null_checks) {
    fc_welford_state_t states[2];
    double values[] = {1.0, 2.0};

    ASSERT_EQ(fc_welford_update_streams(NULL, values, 2), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_update_streams(states, NULL, 2), FC_ERR_INVALID_ARG);
    ASSERT_EQ(fc_welford_update_streams(states, values, 0), FC_ERR_INVALID_ARG);
}

TEST(test_welford_update_streams_nan) {
    fc_welford_state_t states[3];
    for (int i = 0; i < 3; i++) {
        fc_welford_init(&states[i]);
    }

    double values[] = {1.0, NAN, 3.0};
    ASSERT_EQ(fc_welford_update_streams(states, values, 3), FC_ERR_NAN_INPUT);
}

TEST(test_welford_update_streams_infinity) {
    fc_welford_state_t states[3];
    for (int i = 0; i < 3; i++) {
        fc_welford_init(&states[i]);
    }

    double values[] = {1.0, INFINITY, 3.0};
    ASSERT_EQ(fc_welford_update_streams(states, values, 3), FC_ERR_INVALID_ARG);
}

TEST(test_welford_streams_vs_single) {
    // Verify that updating streams gives same result as updating individually
    fc_welford_state_t states_stream[4];
    fc_welford_state_t states_single[4];

    for (int i = 0; i < 4; i++) {
        fc_welford_init(&states_stream[i]);
        fc_welford_init(&states_single[i]);
    }

    // Update using streams
    double vals1[] = {1.0, 2.0, 3.0, 4.0};
    double vals2[] = {5.0, 6.0, 7.0, 8.0};
    double vals3[] = {9.0, 10.0, 11.0, 12.0};

    fc_welford_update_streams(states_stream, vals1, 4);
    fc_welford_update_streams(states_stream, vals2, 4);
    fc_welford_update_streams(states_stream, vals3, 4);

    // Update individually
    fc_welford_update(&states_single[0], 1.0);
    fc_welford_update(&states_single[0], 5.0);
    fc_welford_update(&states_single[0], 9.0);

    fc_welford_update(&states_single[1], 2.0);
    fc_welford_update(&states_single[1], 6.0);
    fc_welford_update(&states_single[1], 10.0);

    fc_welford_update(&states_single[2], 3.0);
    fc_welford_update(&states_single[2], 7.0);
    fc_welford_update(&states_single[2], 11.0);

    fc_welford_update(&states_single[3], 4.0);
    fc_welford_update(&states_single[3], 8.0);
    fc_welford_update(&states_single[3], 12.0);

    // Compare results
    for (int i = 0; i < 4; i++) {
        double mean_stream, mean_single;
        fc_welford_mean(&states_stream[i], &mean_stream);
        fc_welford_mean(&states_single[i], &mean_single);
        FC_TEST_ASSERT_DOUBLE_EQ(mean_stream, mean_single, TEST_TOLERANCE);

        double var_stream, var_single;
        fc_welford_variance(&states_stream[i], &var_stream, 1);
        fc_welford_variance(&states_single[i], &var_single, 1);
        FC_TEST_ASSERT_DOUBLE_EQ(var_stream, var_single, TEST_TOLERANCE);
    }
}

void register_welford_tests(void) {
    RUN_TEST(test_welford_init);
    RUN_TEST(test_welford_update_single);
    RUN_TEST(test_welford_update_nan);
    RUN_TEST(test_welford_update_infinity);
    RUN_TEST(test_welford_mean);
    RUN_TEST(test_welford_variance_sample);
    RUN_TEST(test_welford_variance_population);
    RUN_TEST(test_welford_variance_single_value);
    RUN_TEST(test_welford_stddev);
    RUN_TEST(test_welford_stddev_empty);
    RUN_TEST(test_welford_count);
    RUN_TEST(test_welford_update_batch);
    RUN_TEST(test_welford_update_batch_nan);
    RUN_TEST(test_welford_update_batch_infinity);
    RUN_TEST(test_welford_batch_small_sizes);
    RUN_TEST(test_welford_batch_medium_size);
    RUN_TEST(test_welford_batch_large_size);
    RUN_TEST(test_welford_merge_empty);
    RUN_TEST(test_welford_merge_into_empty);
    RUN_TEST(test_welford_merge_basic);
    RUN_TEST(test_welford_merge_asymmetric);
    RUN_TEST(test_welford_merge_null);
    RUN_TEST(test_welford_multiple_merges);
    RUN_TEST(test_welford_numerical_stability);
    RUN_TEST(test_welford_large_dataset);
    RUN_TEST(test_welford_reset);
    RUN_TEST(test_welford_incremental_vs_batch);
    RUN_TEST(test_welford_simd_sse_path);
    RUN_TEST(test_welford_simd_avx2_path);
    RUN_TEST(test_welford_simd_avx512_path);
    RUN_TEST(test_welford_simd_large_batch);
    RUN_TEST(test_welford_simd_odd_sizes);
    RUN_TEST(test_welford_update_streams_basic);
    RUN_TEST(test_welford_update_streams_sse_width);
    RUN_TEST(test_welford_update_streams_avx2_width);
    RUN_TEST(test_welford_update_streams_avx512_width);
    RUN_TEST(test_welford_update_streams_large);
    RUN_TEST(test_welford_update_streams_variance);
    RUN_TEST(test_welford_update_streams_null_checks);
    RUN_TEST(test_welford_update_streams_nan);
    RUN_TEST(test_welford_update_streams_infinity);
    RUN_TEST(test_welford_streams_vs_single);
}
