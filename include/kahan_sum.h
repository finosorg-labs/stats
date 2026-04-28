/**
 * @file kahan_sum.h
 * @brief Kahan compensated summation for high-precision floating-point accumulation
 *
 * Kahan summation algorithm reduces numerical error in the total obtained by adding
 * a sequence of finite-precision floating-point numbers, compared to naive summation.
 * It maintains a running compensation for lost low-order bits.
 *
 * Algorithm:
 *   sum = 0.0
 *   c = 0.0  // compensation for lost low-order bits
 *   for each x in data:
 *     y = x - c
 *     t = sum + y
 *     c = (t - sum) - y
 *     sum = t
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Thread Safety: Reentrant (no shared state)
 */

#ifndef FC_KAHAN_SUM_H
#define FC_KAHAN_SUM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute sum of array using Kahan compensated summation
 *
 * This function provides higher numerical accuracy than naive summation,
 * especially when summing many numbers or when the magnitudes vary greatly.
 *
 * @param[in] data Input array of double-precision values
 * @param[in] n Number of elements in the array
 * @return Sum of all elements with reduced numerical error
 *
 * @note Returns 0.0 for empty array (n == 0)
 * @note NaN values propagate through the sum
 * @note Thread-safe (no shared state)
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 *
 * Example:
 * @code
 * double data[] = {1e10, 1.0, -1e10, 1.0};
 * double sum = fc_stat_kahan_sum_f64(data, 4);
 * // sum = 2.0 (accurate)
 * // naive sum might give 0.0 due to catastrophic cancellation
 * @endcode
 */
double fc_stat_kahan_sum_f64(const double* data, size_t n);

/**
 * @brief Compute sum of array using Kahan compensated summation (single precision)
 *
 * Single-precision variant of Kahan summation.
 *
 * @param[in] data Input array of single-precision values
 * @param[in] n Number of elements in the array
 * @return Sum of all elements with reduced numerical error
 *
 * @note Returns 0.0f for empty array (n == 0)
 * @note NaN values propagate through the sum
 * @note Thread-safe (no shared state)
 *
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 */
float fc_stat_kahan_sum_f32(const float* data, size_t n);

/**
 * @brief Kahan summation state for incremental updates
 *
 * Maintains the running sum and compensation term for incremental
 * Kahan summation. Useful when data arrives in chunks or streams.
 */
typedef struct {
    double sum;  /**< Running sum */
    double c;    /**< Compensation for lost low-order bits */
} fc_kahan_state_t;

/**
 * @brief Initialize Kahan summation state
 *
 * @param[out] state Pointer to state structure to initialize
 *
 * @note Must be called before using fc_stat_kahan_add_*
 */
void fc_stat_kahan_init(fc_kahan_state_t* state);

/**
 * @brief Add a single value to Kahan sum
 *
 * @param[in,out] state Pointer to Kahan state
 * @param[in] value Value to add
 *
 * @note State must be initialized with fc_stat_kahan_init first
 * @note Thread-safe if each thread uses its own state
 */
void fc_stat_kahan_add(fc_kahan_state_t* state, double value);

/**
 * @brief Add multiple values to Kahan sum
 *
 * @param[in,out] state Pointer to Kahan state
 * @param[in] data Array of values to add
 * @param[in] n Number of values
 *
 * @note State must be initialized with fc_stat_kahan_init first
 * @note Thread-safe if each thread uses its own state
 */
void fc_stat_kahan_add_batch(fc_kahan_state_t* state, const double* data, size_t n);

/**
 * @brief Get current sum from Kahan state
 *
 * @param[in] state Pointer to Kahan state
 * @return Current sum value
 *
 * @note Does not modify state
 */
double fc_stat_kahan_get_sum(const fc_kahan_state_t* state);

#ifdef __cplusplus
}
#endif

#endif /* FC_KAHAN_SUM_H */
