package stats

/*
#include "rank.h"
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// RankMethod represents the tie handling method for ranking
type RankMethod int

const (
	// RankAverage assigns average rank to tied values (default)
	RankAverage RankMethod = C.FC_RANK_AVERAGE
	// RankMin assigns minimum rank to tied values
	RankMin RankMethod = C.FC_RANK_MIN
	// RankMax assigns maximum rank to tied values
	RankMax RankMethod = C.FC_RANK_MAX
	// RankFirst ranks by order of appearance (stable)
	RankFirst RankMethod = C.FC_RANK_FIRST
	// RankDense uses dense ranking (1,2,2,3 instead of 1,2,2,4)
	RankDense RankMethod = C.FC_RANK_DENSE
)

// Rank computes ranks of array elements
//
// Assigns ranks to elements based on their sorted order.
// Smaller values get lower ranks (rank 1 is the smallest).
// Supports multiple tie-handling methods.
//
// Parameters:
//   - data: Input array (not modified)
//   - method: Tie handling method
//
// Returns:
//   - ranks: Array of ranks
//   - error: Error if any
//
// Example (RankAverage):
//
//	Input:  [3.0, 1.0, 2.0, 2.0, 5.0]
//	Output: [4.0, 1.0, 2.5, 2.5, 5.0]
//
// Time complexity: O(n log n)
// Space complexity: O(n)
// Thread safety: Thread-safe
func Rank(data []float64, method RankMethod) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("input array is empty")
	}

	ranks := make([]float64, len(data))

	status := C.fc_stats_rank_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		(*C.double)(unsafe.Pointer(&ranks[0])),
		C.fc_rank_method_t(method),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("rank computation failed with status %d", status)
	}

	return ranks, nil
}

// RankNormalized computes fractional ranks (normalized to [0, 1])
//
// Computes ranks and normalizes them to the range [0, 1].
// Useful for cross-sectional factor analysis where different
// universes have different sizes.
//
// Parameters:
//   - data: Input array (not modified)
//   - method: Tie handling method
//
// Returns:
//   - ranks: Array of normalized ranks
//   - error: Error if any
//
// Example (RankAverage):
//
//	Input:  [3.0, 1.0, 2.0, 2.0, 5.0]
//	Output: [0.75, 0.0, 0.375, 0.375, 1.0]
//
// Time complexity: O(n log n)
// Space complexity: O(n)
// Thread safety: Thread-safe
func RankNormalized(data []float64, method RankMethod) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("input array is empty")
	}

	ranks := make([]float64, len(data))

	status := C.fc_stats_rank_normalized_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		(*C.double)(unsafe.Pointer(&ranks[0])),
		C.fc_rank_method_t(method),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("normalized rank computation failed with status %d", status)
	}

	return ranks, nil
}

// RankBatch computes ranks for multiple arrays (batch processing)
//
// Efficiently computes ranks for multiple independent arrays.
// Useful for cross-sectional factor ranking across multiple factors.
//
// Parameters:
//   - data: Input arrays (num_arrays × n elements, row-major)
//   - n: Number of elements per array
//   - method: Tie handling method
//
// Returns:
//   - ranks: Output arrays for ranks (num_arrays × n elements, row-major)
//   - error: Error if any
//
// Example use case: Rank 500 factors across 5000 stocks
//
//	num_arrays = 500 (factors)
//	n = 5000 (stocks)
//
// Time complexity: O(num_arrays × n log n)
// Space complexity: O(n)
// Thread safety: Thread-safe
func RankBatch(data []float64, n int, method RankMethod) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("input array is empty")
	}

	if n <= 0 {
		return nil, fmt.Errorf("n must be positive")
	}

	if len(data)%n != 0 {
		return nil, fmt.Errorf("data length must be divisible by n")
	}

	numArrays := len(data) / n
	ranks := make([]float64, len(data))

	status := C.fc_stats_rank_batch_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(n),
		C.size_t(numArrays),
		(*C.double)(unsafe.Pointer(&ranks[0])),
		C.fc_rank_method_t(method),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("batch rank computation failed with status %d", status)
	}

	return ranks, nil
}
