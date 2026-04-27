/**
 * @file test_stats.c
 * @brief Test suite registration for stats module
 */

#include "test_framework.h"

/* External test registration functions */
extern void register_mean_var_tests(void);

/**
 * @brief Register all stats module tests
 */
void register_stats_tests(void) {
    register_mean_var_tests();
}
