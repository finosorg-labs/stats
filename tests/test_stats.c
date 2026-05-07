/**
 * @file test_stats.c
 * @brief Test suite registration for stats module
 */

#include "test_framework.h"

/* External test registration functions */
extern void register_mean_var_tests(void);
extern void register_kahan_sum_tests(void);
extern void register_welford_tests(void);
extern void register_covariance_tests(void);
extern void register_quantile_tests(void);
extern void register_rolling_tests(void);
extern void register_rank_tests(void);

/**
 * @brief Register all stats module tests
 */
void register_stats_tests(void) {
    register_mean_var_tests();
    register_kahan_sum_tests();
    register_welford_tests();
    register_covariance_tests();
    register_quantile_tests();
    register_rolling_tests();
    register_rank_tests();
}
