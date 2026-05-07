/**
 * @file test_rank.c
 * @brief Unit tests for rank computation
 */

#include "test_framework.h"
#include "../include/rank.h"
#include <math.h>
#include <string.h>

#define TEST_TOLERANCE 1e-10

/*
 * Test: Basic rank computation with different methods
 */

TEST(test_rank_basic_average) {
    double data[] = {3.0, 1.0, 2.0, 2.0, 5.0};
    double ranks[5];
    double expected[] = {4.0, 1.0, 2.5, 2.5, 5.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_rank_basic_min) {
    double data[] = {3.0, 1.0, 2.0, 2.0, 5.0};
    double ranks[5];
    double expected[] = {4.0, 1.0, 2.0, 2.0, 5.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_MIN);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_rank_basic_max) {
    double data[] = {3.0, 1.0, 2.0, 2.0, 5.0};
    double ranks[5];
    double expected[] = {4.0, 1.0, 3.0, 3.0, 5.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_MAX);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_rank_basic_first) {
    double data[] = {3.0, 1.0, 2.0, 2.0, 5.0};
    double ranks[5];
    double expected[] = {4.0, 1.0, 2.0, 3.0, 5.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_FIRST);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_rank_basic_dense) {
    double data[] = {3.0, 1.0, 2.0, 2.0, 5.0};
    double ranks[5];
    double expected[] = {3.0, 1.0, 2.0, 2.0, 4.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_DENSE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

/*
 * Test: Edge cases
 */

TEST(test_rank_single_element) {
    double data[] = {42.0};
    double ranks[1];

    fc_status_t status = fc_stats_rank_f64(data, 1, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(ranks[0], 1.0, TEST_TOLERANCE);
}

TEST(test_rank_all_equal) {
    double data[] = {5.0, 5.0, 5.0, 5.0};
    double ranks[4];

    fc_status_t status = fc_stats_rank_f64(data, 4, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 4; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], 2.5, TEST_TOLERANCE);
    }
}

TEST(test_rank_all_equal_min) {
    double data[] = {5.0, 5.0, 5.0, 5.0};
    double ranks[4];

    fc_status_t status = fc_stats_rank_f64(data, 4, ranks, FC_RANK_MIN);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 4; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], 1.0, TEST_TOLERANCE);
    }
}

TEST(test_rank_all_equal_dense) {
    double data[] = {5.0, 5.0, 5.0, 5.0};
    double ranks[4];

    fc_status_t status = fc_stats_rank_f64(data, 4, ranks, FC_RANK_DENSE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 4; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], 1.0, TEST_TOLERANCE);
    }
}

TEST(test_rank_sorted_ascending) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double ranks[5];
    double expected[] = {1.0, 2.0, 3.0, 4.0, 5.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_rank_sorted_descending) {
    double data[] = {5.0, 4.0, 3.0, 2.0, 1.0};
    double ranks[5];
    double expected[] = {5.0, 4.0, 3.0, 2.0, 1.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_rank_with_nan) {
    double data[] = {3.0, NAN, 1.0, 2.0, NAN};
    double ranks[5];

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(ranks[0], 3.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(ranks[1], 4.5, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(ranks[2], 1.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(ranks[3], 2.0, TEST_TOLERANCE);
    FC_TEST_ASSERT_DOUBLE_EQ(ranks[4], 4.5, TEST_TOLERANCE);
}

TEST(test_rank_negative_values) {
    double data[] = {-3.0, -1.0, -2.0, 0.0, 1.0};
    double ranks[5];
    double expected[] = {1.0, 3.0, 2.0, 4.0, 5.0};

    fc_status_t status = fc_stats_rank_f64(data, 5, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

/*
 * Test: Normalized ranks
 */

TEST(test_rank_normalized_basic) {
    double data[] = {3.0, 1.0, 2.0, 2.0, 5.0};
    double ranks[5];
    double expected[] = {0.75, 0.0, 0.375, 0.375, 1.0};

    fc_status_t status = fc_stats_rank_normalized_f64(data, 5, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

TEST(test_rank_normalized_single) {
    double data[] = {42.0};
    double ranks[1];

    fc_status_t status = fc_stats_rank_normalized_f64(data, 1, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(ranks[0], 0.5, TEST_TOLERANCE);
}

TEST(test_rank_normalized_two_elements) {
    double data[] = {1.0, 2.0};
    double ranks[2];
    double expected[] = {0.0, 1.0};

    fc_status_t status = fc_stats_rank_normalized_f64(data, 2, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 2; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

/*
 * Test: Batch ranking
 */

TEST(test_rank_batch_basic) {
    double data[] = {
        3.0, 1.0, 2.0, 2.0, 5.0,
        10.0, 20.0, 15.0, 15.0, 5.0
    };
    double ranks[10];
    double expected[] = {
        4.0, 1.0, 2.5, 2.5, 5.0,
        2.0, 5.0, 3.5, 3.5, 1.0
    };

    fc_status_t status = fc_stats_rank_batch_f64(data, 5, 2, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < 10; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], expected[i], TEST_TOLERANCE);
    }
}

/*
 * Test: Large arrays
 */

TEST(test_rank_large_array) {
    const size_t n = 10000;
    double* data = (double*)malloc(n * sizeof(double));
    double* ranks = (double*)malloc(n * sizeof(double));
    ASSERT_NOT_NULL(data);
    ASSERT_NOT_NULL(ranks);

    for (size_t i = 0; i < n; i++) {
        data[i] = (double)(n - i);
    }

    fc_status_t status = fc_stats_rank_f64(data, n, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_OK);

    for (size_t i = 0; i < n; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(ranks[i], (double)(n - i), TEST_TOLERANCE);
    }

    free(data);
    free(ranks);
}

/*
 * Test: Error handling
 */

TEST(test_rank_null_input) {
    double ranks[5];
    fc_status_t status = fc_stats_rank_f64(NULL, 5, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_rank_null_output) {
    double data[] = {1.0, 2.0, 3.0};
    fc_status_t status = fc_stats_rank_f64(data, 3, NULL, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_rank_zero_length) {
    double data[] = {1.0};
    double ranks[1];
    fc_status_t status = fc_stats_rank_f64(data, 0, ranks, FC_RANK_AVERAGE);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

/*
 * Test registration
 */

void register_rank_tests(void) {
    RUN_TEST(test_rank_basic_average);
    RUN_TEST(test_rank_basic_min);
    RUN_TEST(test_rank_basic_max);
    RUN_TEST(test_rank_basic_first);
    RUN_TEST(test_rank_basic_dense);

    RUN_TEST(test_rank_single_element);
    RUN_TEST(test_rank_all_equal);
    RUN_TEST(test_rank_all_equal_min);
    RUN_TEST(test_rank_all_equal_dense);
    RUN_TEST(test_rank_sorted_ascending);
    RUN_TEST(test_rank_sorted_descending);
    RUN_TEST(test_rank_with_nan);
    RUN_TEST(test_rank_negative_values);

    RUN_TEST(test_rank_normalized_basic);
    RUN_TEST(test_rank_normalized_single);
    RUN_TEST(test_rank_normalized_two_elements);

    RUN_TEST(test_rank_batch_basic);

    RUN_TEST(test_rank_large_array);

    RUN_TEST(test_rank_null_input);
    RUN_TEST(test_rank_null_output);
    RUN_TEST(test_rank_zero_length);
}
