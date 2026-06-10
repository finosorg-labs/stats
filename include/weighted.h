/**
 * @file weighted.h
 * @brief Weighted statistical computations
 *
 * Provides weighted mean, population variance, and standard deviation for
 * double-precision data. Weights are non-negative and may be zero as long as
 * the total weight is positive.
 */

#ifndef FC_STATS_weighted_H
#define FC_STATS_weighted_H

#include "error.h"
#include "platform.h"

FC_BEGIN_DECLS

/**
 * @brief Compute weighted mean of a single array
 *
 * Computes Σ(data[i] * weights[i]) / Σ(weights[i]) using Kahan compensated
 * summation for improved numerical stability.
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 *
 * @param data Input array (must not be NULL)
 * @param weights Weight array (must not be NULL, all weights must be >= 0)
 * @param n Number of elements (must be > 0)
 * @param mean Output weighted mean value (must not be NULL)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 * @note Returns FC_ERR_NAN_INPUT if any data value or weight is NaN
 * @note Returns FC_ERR_INVALID_ARG if total weight is zero
 */
FC_API fc_status_t
fc_stats_weighted_mean_f64(const double* data, const double* weights, size_t n, double* mean);

/**
 * @brief Compute weighted population variance of a single array
 *
 * Computes Σ(weights[i] * (data[i] - mean)^2) / Σ(weights[i]). This function
 * intentionally computes population weighted variance only.
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 *
 * @param data Input array (must not be NULL)
 * @param weights Weight array (must not be NULL, all weights must be >= 0)
 * @param n Number of elements (must be > 0)
 * @param variance Output weighted population variance (must not be NULL)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 * @note Returns FC_ERR_NAN_INPUT if any data value or weight is NaN
 * @note Returns FC_ERR_INVALID_ARG if total weight is zero
 */
FC_API fc_status_t fc_stats_weighted_variance_f64(
    const double* data,
    const double* weights,
    size_t n,
    double* variance
);

/**
 * @brief Compute weighted mean and population variance simultaneously
 *
 * More efficient than calling weighted mean and weighted variance separately.
 * The variance is population weighted variance:
 * Σ(weights[i] * (data[i] - mean)^2) / Σ(weights[i]).
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 *
 * @param data Input array (must not be NULL)
 * @param weights Weight array (must not be NULL, all weights must be >= 0)
 * @param n Number of elements (must be > 0)
 * @param mean Output weighted mean value (must not be NULL)
 * @param variance Output weighted population variance (must not be NULL)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 * @note Returns FC_ERR_NAN_INPUT if any data value or weight is NaN
 * @note Returns FC_ERR_INVALID_ARG if total weight is zero
 */
FC_API fc_status_t fc_stats_weighted_mean_variance_f64(
    const double* data,
    const double* weights,
    size_t n,
    double* mean,
    double* variance
);

/**
 * @brief Compute weighted population standard deviation of a single array
 *
 * Convenience function that computes sqrt(weighted population variance).
 *
 * Time complexity: O(n)
 * Space complexity: O(1)
 *
 * @param data Input array (must not be NULL)
 * @param weights Weight array (must not be NULL, all weights must be >= 0)
 * @param n Number of elements (must be > 0)
 * @param stddev Output weighted population standard deviation (must not be NULL)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 * @note Returns FC_ERR_NAN_INPUT if any data value or weight is NaN
 * @note Returns FC_ERR_INVALID_ARG if total weight is zero
 */
FC_API fc_status_t
fc_stats_weighted_stddev_f64(const double* data, const double* weights, size_t n, double* stddev);

/**
 * @brief Batch compute weighted means for flat groups
 *
 * Processes flat data laid out as n_groups contiguous groups, each with
 * group_size elements. The weights array uses the same layout.
 *
 * Time complexity: O(n_groups * group_size)
 * Space complexity: O(1)
 *
 * @param means Output array of weighted means (must not be NULL, size >= n_groups)
 * @param data Flat input array (must not be NULL, size >= n_groups * group_size)
 * @param weights Flat weight array (must not be NULL, size >= n_groups * group_size)
 * @param n_groups Number of groups (must be > 0)
 * @param group_size Number of elements per group (must be > 0)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 */
FC_API fc_status_t fc_stats_weighted_mean_batch_f64(
    double* means,
    const double* data,
    const double* weights,
    size_t n_groups,
    size_t group_size
);

/**
 * @brief Batch compute weighted population variances for flat groups
 *
 * Time complexity: O(n_groups * group_size)
 * Space complexity: O(1)
 *
 * @param variances Output array of weighted variances (must not be NULL, size >= n_groups)
 * @param data Flat input array (must not be NULL, size >= n_groups * group_size)
 * @param weights Flat weight array (must not be NULL, size >= n_groups * group_size)
 * @param n_groups Number of groups (must be > 0)
 * @param group_size Number of elements per group (must be > 0)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 */
FC_API fc_status_t fc_stats_weighted_variance_batch_f64(
    double* variances,
    const double* data,
    const double* weights,
    size_t n_groups,
    size_t group_size
);

/**
 * @brief Batch compute weighted means and population variances for flat groups
 *
 * Time complexity: O(n_groups * group_size)
 * Space complexity: O(1)
 *
 * @param means Output array of weighted means (must not be NULL, size >= n_groups)
 * @param variances Output array of weighted variances (must not be NULL, size >= n_groups)
 * @param data Flat input array (must not be NULL, size >= n_groups * group_size)
 * @param weights Flat weight array (must not be NULL, size >= n_groups * group_size)
 * @param n_groups Number of groups (must be > 0)
 * @param group_size Number of elements per group (must be > 0)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 */
FC_API fc_status_t fc_stats_weighted_mean_variance_batch_f64(
    double* means,
    double* variances,
    const double* data,
    const double* weights,
    size_t n_groups,
    size_t group_size
);

/**
 * @brief Batch compute weighted population standard deviations for flat groups
 *
 * Time complexity: O(n_groups * group_size)
 * Space complexity: O(1)
 *
 * @param stddevs Output array of weighted standard deviations (must not be NULL, size >= n_groups)
 * @param data Flat input array (must not be NULL, size >= n_groups * group_size)
 * @param weights Flat weight array (must not be NULL, size >= n_groups * group_size)
 * @param n_groups Number of groups (must be > 0)
 * @param group_size Number of elements per group (must be > 0)
 *
 * @return FC_OK on success, error code on failure
 *
 * @note Thread-safe
 */
FC_API fc_status_t fc_stats_weighted_stddev_batch_f64(
    double* stddevs,
    const double* data,
    const double* weights,
    size_t n_groups,
    size_t group_size
);

FC_END_DECLS

#endif /* FC_STATS_weighted_H */
