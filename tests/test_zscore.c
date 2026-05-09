#include "zscore.h"
#include "test_framework.h"
#include <math.h>
#include <string.h>

#define TEST_TOLERANCE 1e-12

TEST(test_zscore_basic) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double out[5];

    fc_status_t status = fc_stat_zscore_f64(out, data, 5, 1);
    ASSERT_EQ(status, FC_OK);

    double expected_mean = 0.0;
    double sum_sq = 0.0;
    for (int i = 0; i < 5; i++) {
        expected_mean += out[i];
        sum_sq += out[i] * out[i];
    }
    expected_mean /= 5.0;
    double expected_std = sqrt(sum_sq / 4.0);

    FC_TEST_ASSERT_DOUBLE_EQ(expected_mean, 0.0, 1e-10);
    FC_TEST_ASSERT_DOUBLE_EQ(expected_std, 1.0, 1e-10);

    FC_TEST_ASSERT_DOUBLE_EQ(out[0], -1.2649110640673518, 1e-10);
    FC_TEST_ASSERT_DOUBLE_EQ(out[2], 0.0, 1e-10);
    FC_TEST_ASSERT_DOUBLE_EQ(out[4], 1.2649110640673518, 1e-10);
}

TEST(test_zscore_population_vs_sample) {
    double data[] = {10.0, 20.0, 30.0, 40.0};
    double out_sample[4];
    double out_pop[4];

    fc_stat_zscore_f64(out_sample, data, 4, 1);
    fc_stat_zscore_f64(out_pop, data, 4, 0);

    for (int i = 0; i < 4; i++) {
        FC_TEST_ASSERT_MSG(fabs(out_sample[i]) < fabs(out_pop[i]) ||
                   (fabs(out_sample[i]) < 1e-10 && fabs(out_pop[i]) < 1e-10),
                   "Sample std is larger, so z-scores should be smaller in absolute value");
    }
}

TEST(test_zscore_constant_data) {
    double data[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    double out[5];

    fc_status_t status = fc_stat_zscore_f64(out, data, 5, 1);
    ASSERT_EQ(status, FC_OK);

    for (int i = 0; i < 5; i++) {
        FC_TEST_ASSERT_MSG(isnan(out[i]), "Constant data should produce NaN");
    }
}

TEST(test_zscore_single_element) {
    double data[] = {42.0};
    double out[1];

    fc_status_t status = fc_stat_zscore_f64(out, data, 1, 1);
    ASSERT_EQ(status, FC_OK);
    FC_TEST_ASSERT_DOUBLE_EQ(out[0], 0.0, 1e-10);
}

TEST(test_zscore_empty) {
    double out[1] = {0.0};
    double data[1] = {0.0};

    fc_status_t status = fc_stat_zscore_f64(out, data, 0, 1);
    ASSERT_EQ(status, FC_OK);
}

TEST(test_zscore_null_pointers) {
    double data[5] = {1, 2, 3, 4, 5};
    double out[5];

    fc_status_t status = fc_stat_zscore_f64(NULL, data, 5, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stat_zscore_f64(out, NULL, 5, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_zscore_inplace) {
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double expected[5];

    fc_stat_zscore_f64(expected, data, 5, 1);
    fc_stat_zscore_f64(data, data, 5, 1);

    for (int i = 0; i < 5; i++) {
        FC_TEST_ASSERT_DOUBLE_EQ(data[i], expected[i], 1e-10);
    }
}

TEST(test_zscore_negative_values) {
    double data[] = {-10.0, -5.0, 0.0, 5.0, 10.0};
    double out[5];

    fc_status_t status = fc_stat_zscore_f64(out, data, 5, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(out[2], 0.0, 1e-10);
    FC_TEST_ASSERT_MSG(out[0] < 0.0, "Negative values should have negative z-scores");
    FC_TEST_ASSERT_MSG(out[4] > 0.0, "Positive values should have positive z-scores");
}

TEST(test_zscore_large_values) {
    double data[] = {1e10, 2e10, 3e10, 4e10, 5e10};
    double out[5];

    fc_status_t status = fc_stat_zscore_f64(out, data, 5, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(out[2], 0.0, 1e-6);
}

TEST(test_zscore_batch_basic) {
    double data[] = {
        1.0, 2.0, 3.0, 4.0,
        10.0, 20.0, 30.0, 40.0,
        100.0, 200.0, 300.0, 400.0
    };
    double out[12];

    fc_status_t status = fc_stat_zscore_batch_f64(out, data, 3, 4, 1);
    ASSERT_EQ(status, FC_OK);

    for (int g = 0; g < 3; g++) {
        double mean = 0.0;
        for (int i = 0; i < 4; i++) {
            mean += out[g * 4 + i];
        }
        mean /= 4.0;
        FC_TEST_ASSERT_DOUBLE_EQ(mean, 0.0, 1e-10);
    }
}

TEST(test_zscore_batch_null_pointers) {
    double data[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    double out[8];

    fc_status_t status = fc_stat_zscore_batch_f64(NULL, data, 2, 4, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);

    status = fc_stat_zscore_batch_f64(out, NULL, 2, 4, 1);
    ASSERT_EQ(status, FC_ERR_INVALID_ARG);
}

TEST(test_zscore_batch_empty) {
    double out[1] = {0.0};
    double data[1] = {0.0};

    fc_status_t status = fc_stat_zscore_batch_f64(out, data, 0, 4, 1);
    ASSERT_EQ(status, FC_OK);

    status = fc_stat_zscore_batch_f64(out, data, 4, 0, 1);
    ASSERT_EQ(status, FC_OK);
}

TEST(test_zscore_numerical_stability) {
    double data[] = {1e15, 1e15 + 1, 1e15 + 2, 1e15 + 3, 1e15 + 4};
    double out[5];

    fc_status_t status = fc_stat_zscore_f64(out, data, 5, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(out[2], 0.0, 1e-6);
}

TEST(test_zscore_simd_alignment) {
    double data[100];
    double out[100];

    for (int i = 0; i < 100; i++) {
        data[i] = (double)i;
    }

    fc_status_t status = fc_stat_zscore_f64(out, data, 100, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(out[49], -0.017234549688642783, 1e-6);
    FC_TEST_ASSERT_DOUBLE_EQ(out[50], 0.017234549688642783, 1e-6);
}

TEST(test_zscore_odd_size) {
    double data[99];
    double out[99];

    for (int i = 0; i < 99; i++) {
        data[i] = (double)i;
    }

    fc_status_t status = fc_stat_zscore_f64(out, data, 99, 1);
    ASSERT_EQ(status, FC_OK);

    FC_TEST_ASSERT_DOUBLE_EQ(out[49], 0.0, 1e-6);
}

void register_zscore_tests(void) {
    RUN_TEST(test_zscore_basic);
    RUN_TEST(test_zscore_population_vs_sample);
    RUN_TEST(test_zscore_constant_data);
    RUN_TEST(test_zscore_single_element);
    RUN_TEST(test_zscore_empty);
    RUN_TEST(test_zscore_null_pointers);
    RUN_TEST(test_zscore_inplace);
    RUN_TEST(test_zscore_negative_values);
    RUN_TEST(test_zscore_large_values);
    RUN_TEST(test_zscore_batch_basic);
    RUN_TEST(test_zscore_batch_null_pointers);
    RUN_TEST(test_zscore_batch_empty);
    RUN_TEST(test_zscore_numerical_stability);
    RUN_TEST(test_zscore_simd_alignment);
    RUN_TEST(test_zscore_odd_size);
}
