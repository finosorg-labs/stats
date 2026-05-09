package stats

/*
#include "zscore.h"
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// ZScore computes Z-scores (standardization) for the input data.
// Z-score = (x - mean) / std
//
// Parameters:
//   - data: Input data slice
//   - sample: If true, use sample standard deviation (N-1), otherwise population (N)
//
// Returns:
//   - Slice of Z-scores with same length as input
//   - Error if operation fails
//
// Note: If standard deviation is zero or very close to zero, returns NaN values.
//
// Example:
//
//	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
//	zscores, err := stats.ZScore(data, true)
//	// zscores ≈ [-1.414, -0.707, 0.0, 0.707, 1.414]
func ZScore(data []float64, sample bool) ([]float64, error) {
	if len(data) == 0 {
		return []float64{}, nil
	}

	out := make([]float64, len(data))

	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stat_zscore_f64(
		(*C.double)(unsafe.Pointer(&out[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		sampleFlag,
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("fc_stat_zscore_f64 failed with status %d", status)
	}

	return out, nil
}

// ZScoreInPlace computes Z-scores in-place, modifying the input slice.
//
// Parameters:
//   - data: Input/output data slice (will be modified)
//   - sample: If true, use sample standard deviation (N-1), otherwise population (N)
//
// Returns:
//   - Error if operation fails
//
// Note: This is more memory-efficient than ZScore as it doesn't allocate a new slice.
func ZScoreInPlace(data []float64, sample bool) error {
	if len(data) == 0 {
		return nil
	}

	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stat_zscore_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		sampleFlag,
	)

	if status != C.FC_OK {
		return fmt.Errorf("fc_stat_zscore_f64 failed with status %d", status)
	}

	return nil
}

// ZScoreBatch computes Z-scores for multiple groups independently.
// Each group is standardized to have mean=0 and std=1 within that group.
//
// Parameters:
//   - data: Input data as flat slice (n_groups * group_size elements)
//   - nGroups: Number of groups
//   - groupSize: Number of elements per group
//   - sample: If true, use sample standard deviation (N-1), otherwise population (N)
//
// Returns:
//   - Slice of Z-scores with same length as input
//   - Error if operation fails
//
// Note: Data layout is [group0_elem0, group0_elem1, ..., group1_elem0, ...]
//
// Example:
//
//	// 3 groups, 4 elements each
//	data := []float64{1, 2, 3, 4, 10, 20, 30, 40, 100, 200, 300, 400}
//	zscores, err := stats.ZScoreBatch(data, 3, 4, true)
//	// Each group independently standardized
func ZScoreBatch(data []float64, nGroups, groupSize int, sample bool) ([]float64, error) {
	if len(data) == 0 || nGroups == 0 || groupSize == 0 {
		return []float64{}, nil
	}

	if len(data) != nGroups*groupSize {
		return nil, fmt.Errorf("data length %d does not match nGroups*groupSize %d", len(data), nGroups*groupSize)
	}

	out := make([]float64, len(data))

	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stat_zscore_batch_f64(
		(*C.double)(unsafe.Pointer(&out[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(nGroups),
		C.size_t(groupSize),
		sampleFlag,
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("fc_stat_zscore_batch_f64 failed with status %d", status)
	}

	return out, nil
}

// ZScoreBatchInPlace computes Z-scores for multiple groups in-place.
//
// Parameters:
//   - data: Input/output data slice (will be modified)
//   - nGroups: Number of groups
//   - groupSize: Number of elements per group
//   - sample: If true, use sample standard deviation (N-1), otherwise population (N)
//
// Returns:
//   - Error if operation fails
func ZScoreBatchInPlace(data []float64, nGroups, groupSize int, sample bool) error {
	if len(data) == 0 || nGroups == 0 || groupSize == 0 {
		return nil
	}

	if len(data) != nGroups*groupSize {
		return fmt.Errorf("data length %d does not match nGroups*groupSize %d", len(data), nGroups*groupSize)
	}

	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stat_zscore_batch_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(nGroups),
		C.size_t(groupSize),
		sampleFlag,
	)

	if status != C.FC_OK {
		return fmt.Errorf("fc_stat_zscore_batch_f64 failed with status %d", status)
	}

	return nil
}
