/**
 * @file topk.h
 * @brief High-performance Top-K selection algorithms
 *
 * Provides optimized selection of K largest/smallest elements from arrays:
 * - QuickSelect for average O(n) complexity
 * - Heap-based approach for small K (K < 100)
 * - Partial sorting for medium K (100 <= K < n/10)
 * - Full sort for large K (K >= n/10)
 *
 * Algorithm selection strategy:
 * - Small K (< 100): Min/Max heap
 * - Medium K (100 to n/10): QuickSelect with partial sort
 * - Large K (>= n/10): Full sort (more cache-friendly)
 *
 * Batch processing features:
 * - OpenMP parallel execution for large batches (>= 4 arrays)
 * - Automatic fallback to sequential for small batches
 * - Dynamic scheduling for load balancing
 */

#ifndef FC_TOPK_H
#define FC_TOPK_H

#include <error.h>
#include <platform.h>
#include <stddef.h>
#include <stdint.h>

FC_BEGIN_DECLS

/**
 * @brief Select K largest elements from float64 array
 *
 * Partially sorts the array so that the K largest elements are at the beginning
 * in descending order. The remaining elements are unordered.
 *
 * Uses adaptive algorithm selection:
 * - K < 100: Heap-based selection O(n log K)
 * - 100 <= K < n/10: QuickSelect O(n) average
 * - K >= n/10: Full sort O(n log n)
 *
 * Time complexity: O(n) average for small/medium K, O(n log n) for large K
 * Space complexity: O(1) in-place operation
 *
 * @param data Array to select from (modified in-place)
 * @param n Number of elements
 * @param k Number of largest elements to select (must be <= n)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if k > n or data is NULL
 *
 * Example:
 *   double data[] = {3.0, 1.0, 4.0, 1.0, 5.0, 9.0, 2.0, 6.0};
 *   fc_topk_largest_f64(data, 8, 3);
 *   // First 3 elements are now: [9.0, 6.0, 5.0] (in descending order)
 */
FC_API fc_status_t fc_topk_largest_f64(double* data, size_t n, size_t k);

/**
 * @brief Select K smallest elements from float64 array
 *
 * Partially sorts the array so that the K smallest elements are at the beginning
 * in sorted order. The remaining elements are unordered.
 *
 * @param data Array to select from (modified in-place)
 * @param n Number of elements
 * @param k Number of smallest elements to select (must be <= n)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if k > n or data is NULL
 *
 * Example:
 *   double data[] = {3.0, 1.0, 4.0, 1.0, 5.0, 9.0, 2.0, 6.0};
 *   fc_topk_smallest_f64(data, 8, 3);
 *   // First 3 elements are now: [1.0, 1.0, 2.0] (in sorted order)
 */
FC_API fc_status_t fc_topk_smallest_f64(double* data, size_t n, size_t k);

/**
 * @brief Select K largest elements from float32 array
 *
 * Same algorithm selection as fc_topk_largest_f64 but optimized for 32-bit floats.
 *
 * @param data Array to select from (modified in-place)
 * @param n Number of elements
 * @param k Number of largest elements to select (must be <= n)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if k > n or data is NULL
 */
FC_API fc_status_t fc_topk_largest_f32(float* data, size_t n, size_t k);

/**
 * @brief Select K smallest elements from float32 array
 *
 * @param data Array to select from (modified in-place)
 * @param n Number of elements
 * @param k Number of smallest elements to select (must be <= n)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if k > n or data is NULL
 */
FC_API fc_status_t fc_topk_smallest_f32(float* data, size_t n, size_t k);

/**
 * @brief Select K largest elements with indices from float64 array
 *
 * Returns both the K largest values and their original indices.
 * The output arrays must be pre-allocated by the caller.
 *
 * @param data Input array (not modified)
 * @param n Number of elements
 * @param k Number of largest elements to select (must be <= n)
 * @param out_values Output array for K largest values (must have size >= k)
 * @param out_indices Output array for original indices (must have size >= k)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if parameters are invalid
 *
 * Example:
 *   double data[] = {3.0, 1.0, 4.0, 1.0, 5.0};
 *   double values[2];
 *   size_t indices[2];
 *   fc_topk_largest_f64_indexed(data, 5, 2, values, indices);
 *   // values: [5.0, 4.0], indices: [4, 2]
 */
FC_API fc_status_t fc_topk_largest_f64_indexed(
    const double* data,
    size_t n,
    size_t k,
    double* out_values,
    size_t* out_indices
);

/**
 * @brief Select K smallest elements with indices from float64 array
 *
 * @param data Input array (not modified)
 * @param n Number of elements
 * @param k Number of smallest elements to select (must be <= n)
 * @param out_values Output array for K smallest values (must have size >= k)
 * @param out_indices Output array for original indices (must have size >= k)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if parameters are invalid
 */
FC_API fc_status_t fc_topk_smallest_f64_indexed(
    const double* data,
    size_t n,
    size_t k,
    double* out_values,
    size_t* out_indices
);

/**
 * @brief Select K largest elements with indices from float32 array
 *
 * @param data Input array (not modified)
 * @param n Number of elements
 * @param k Number of largest elements to select (must be <= n)
 * @param out_values Output array for K largest values (must have size >= k)
 * @param out_indices Output array for original indices (must have size >= k)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if parameters are invalid
 */
FC_API fc_status_t fc_topk_largest_f32_indexed(
    const float* data,
    size_t n,
    size_t k,
    float* out_values,
    size_t* out_indices
);

/**
 * @brief Select K smallest elements with indices from float32 array
 *
 * @param data Input array (not modified)
 * @param n Number of elements
 * @param k Number of smallest elements to select (must be <= n)
 * @param out_values Output array for K smallest values (must have size >= k)
 * @param out_indices Output array for original indices (must have size >= k)
 * @return FC_OK on success, FC_ERR_INVALID_ARG if parameters are invalid
 */
FC_API fc_status_t fc_topk_smallest_f32_indexed(
    const float* data,
    size_t n,
    size_t k,
    float* out_values,
    size_t* out_indices
);

/**
 * @brief Batch Top-K selection for multiple independent float64 arrays
 *
 * Processes multiple Top-K selections in a single call, reducing cgo overhead
 * for batch operations. Each array is processed independently with its own size and K.
 *
 * Performance optimizations:
 * - Reduces cgo call overhead (e.g., 2000 calls -> 2 calls for 1000 symbols)
 * - OpenMP parallel execution for large batches (>= 4 arrays) when available
 * - Dynamic work scheduling for optimal load balancing
 * - Cache-friendly memory access patterns
 *
 * This is especially beneficial when processing many symbols in parallel (e.g.,
 * market-wide aggregation) as it amortizes the cgo call overhead across all arrays.
 *
 * Time complexity: O(sum(n_i)) average for all arrays
 * Space complexity: O(1) - operates in-place on each array
 * Parallelization: Automatic with OpenMP (>= 4 arrays), sequential otherwise
 *
 * @param batch_data Array of pointers to data arrays (each modified in-place)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @return FC_OK on success, error code if any batch fails
 *
 * Example:
 *   double* arrays[3] = {array1, array2, array3};
 *   size_t sizes[3] = {100, 200, 150};
 *   size_t ks[3] = {10, 20, 15};
 *   fc_topk_largest_f64_batch(arrays, sizes, ks, 3);
 *   // Each array now has its top K elements at the front
 */
FC_API fc_status_t fc_topk_largest_f64_batch(
    double** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches
);

/**
 * @brief Batch Top-K smallest selection for multiple independent float64 arrays
 *
 * @param batch_data Array of pointers to data arrays (each modified in-place)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @return FC_OK on success, error code if any batch fails
 */
FC_API fc_status_t fc_topk_smallest_f64_batch(
    double** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches
);

/**
 * @brief Batch Top-K largest selection for multiple independent float32 arrays
 *
 * @param batch_data Array of pointers to data arrays (each modified in-place)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @return FC_OK on success, error code if any batch fails
 */
FC_API fc_status_t fc_topk_largest_f32_batch(
    float** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches
);

/**
 * @brief Batch Top-K smallest selection for multiple independent float32 arrays
 *
 * @param batch_data Array of pointers to data arrays (each modified in-place)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @return FC_OK on success, error code if any batch fails
 */
FC_API fc_status_t fc_topk_smallest_f32_batch(
    float** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches
);

/**
 * @brief Batch Top-K largest selection with indices for multiple float64 arrays
 *
 * Processes multiple Top-K selections in a single call, preserving original data.
 * Unlike the in-place batch functions, this version does not modify input arrays.
 *
 * Each output array must be pre-allocated by the caller:
 * - out_values[i] must have size >= batch_k[i]
 * - out_indices[i] must have size >= batch_k[i]
 *
 * Performance optimizations:
 * - Reduces cgo call overhead for batch operations
 * - OpenMP parallel execution for large batches (>= 4 arrays) when available
 * - Dynamic work scheduling for optimal load balancing
 *
 * @param batch_data Array of pointers to input data arrays (not modified)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @param out_values Array of pointers to output value arrays (pre-allocated)
 * @param out_indices Array of pointers to output index arrays (pre-allocated)
 * @return FC_OK on success, error code if any batch fails
 *
 * Example:
 *   const double* inputs[2] = {array1, array2};
 *   size_t sizes[2] = {100, 200};
 *   size_t ks[2] = {10, 20};
 *   double* values[2] = {malloc(10*sizeof(double)), malloc(20*sizeof(double))};
 *   size_t* indices[2] = {malloc(10*sizeof(size_t)), malloc(20*sizeof(size_t))};
 *   fc_topk_largest_f64_indexed_batch(inputs, sizes, ks, 2, values, indices);
 *   // values[0] contains top 10 from array1, indices[0] contains their positions
 */
FC_API fc_status_t fc_topk_largest_f64_indexed_batch(
    const double** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches,
    double** out_values,
    size_t** out_indices
);

/**
 * @brief Batch Top-K smallest selection with indices for multiple float64 arrays
 *
 * @param batch_data Array of pointers to input data arrays (not modified)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @param out_values Array of pointers to output value arrays (pre-allocated)
 * @param out_indices Array of pointers to output index arrays (pre-allocated)
 * @return FC_OK on success, error code if any batch fails
 */
FC_API fc_status_t fc_topk_smallest_f64_indexed_batch(
    const double** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches,
    double** out_values,
    size_t** out_indices
);

/**
 * @brief Batch Top-K largest selection with indices for multiple float32 arrays
 *
 * @param batch_data Array of pointers to input data arrays (not modified)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @param out_values Array of pointers to output value arrays (pre-allocated)
 * @param out_indices Array of pointers to output index arrays (pre-allocated)
 * @return FC_OK on success, error code if any batch fails
 */
FC_API fc_status_t fc_topk_largest_f32_indexed_batch(
    const float** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches,
    float** out_values,
    size_t** out_indices
);

/**
 * @brief Batch Top-K smallest selection with indices for multiple float32 arrays
 *
 * @param batch_data Array of pointers to input data arrays (not modified)
 * @param batch_sizes Array of sizes for each data array
 * @param batch_k Array of K values for each array
 * @param num_batches Number of arrays to process
 * @param out_values Array of pointers to output value arrays (pre-allocated)
 * @param out_indices Array of pointers to output index arrays (pre-allocated)
 * @return FC_OK on success, error code if any batch fails
 */
FC_API fc_status_t fc_topk_smallest_f32_indexed_batch(
    const float** batch_data,
    const size_t* batch_sizes,
    const size_t* batch_k,
    size_t num_batches,
    float** out_values,
    size_t** out_indices
);

FC_END_DECLS

#endif /* FC_TOPK_H */
