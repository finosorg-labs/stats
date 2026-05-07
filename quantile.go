package stats

/*
#include "quantile.h"
#include "error.h"
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"math"
	"unsafe"
)

// Quantile computes a single quantile from the data array.
// The input slice will be modified (partially sorted).
//
// Parameters:
//   - data: Input data slice (will be modified)
//   - q: Quantile to compute (0.0 to 1.0, e.g., 0.5 for median)
//
// Returns the quantile value and any error encountered.
//
// Time complexity: O(n) average case
// Space complexity: O(1)
//
// Note: The input slice is modified during computation.
func Quantile(data []float64, q float64) (float64, error) {
	if len(data) == 0 {
		return 0, fmt.Errorf("empty data slice")
	}

	if q < 0.0 || q > 1.0 {
		return 0, fmt.Errorf("quantile must be between 0.0 and 1.0, got %f", q)
	}

	// Check for NaN values
	for _, v := range data {
		if math.IsNaN(v) {
			return 0, fmt.Errorf("data contains NaN values")
		}
	}

	var result C.double
	status := C.fc_stats_quantile_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.double(q),
		&result,
	)

	if status != C.FC_OK {
		return 0, fmt.Errorf("quantile computation failed with status %d", status)
	}

	return float64(result), nil
}

// Quantiles computes multiple quantiles from the data array.
// The input slice will be modified (partially sorted).
//
// Parameters:
//   - data: Input data slice (will be modified)
//   - quantiles: Slice of quantile values to compute (each 0.0 to 1.0)
//
// Returns a slice of quantile values and any error encountered.
//
// Time complexity: O(n * k) where k is the number of quantiles
// Space complexity: O(k)
//
// Note: The input slice is modified during computation.
// For best performance, provide quantiles in ascending order.
func Quantiles(data []float64, quantiles []float64) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("empty data slice")
	}

	if len(quantiles) == 0 {
		return nil, fmt.Errorf("empty quantiles slice")
	}

	// Validate quantile values
	for i, q := range quantiles {
		if q < 0.0 || q > 1.0 {
			return nil, fmt.Errorf("quantile[%d] must be between 0.0 and 1.0, got %f", i, q)
		}
	}

	// Check for NaN values
	for _, v := range data {
		if math.IsNaN(v) {
			return nil, fmt.Errorf("data contains NaN values")
		}
	}

	results := make([]float64, len(quantiles))
	status := C.fc_stats_quantiles_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		(*C.double)(unsafe.Pointer(&quantiles[0])),
		C.size_t(len(quantiles)),
		(*C.double)(unsafe.Pointer(&results[0])),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("quantiles computation failed with status %d", status)
	}

	return results, nil
}

// Median computes the median (50th percentile) of the data array.
// The input slice will be modified (partially sorted).
//
// Parameters:
//   - data: Input data slice (will be modified)
//
// Returns the median value and any error encountered.
//
// Time complexity: O(n) average case
// Space complexity: O(1)
//
// Note: The input slice is modified during computation.
func Median(data []float64) (float64, error) {
	if len(data) == 0 {
		return 0, fmt.Errorf("empty data slice")
	}

	// Check for NaN values
	for _, v := range data {
		if math.IsNaN(v) {
			return 0, fmt.Errorf("data contains NaN values")
		}
	}

	var result C.double
	status := C.fc_stats_median_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		&result,
	)

	if status != C.FC_OK {
		return 0, fmt.Errorf("median computation failed with status %d", status)
	}

	return float64(result), nil
}

// Quartiles computes the three quartiles (Q1, Q2/median, Q3) of the data array.
// The input slice will be modified (partially sorted).
//
// Parameters:
//   - data: Input data slice (will be modified)
//
// Returns Q1 (25th percentile), Q2 (50th percentile/median), Q3 (75th percentile),
// and any error encountered.
//
// Time complexity: O(n)
// Space complexity: O(1)
//
// Note: The input slice is modified during computation.
func Quartiles(data []float64) (q1, q2, q3 float64, err error) {
	if len(data) == 0 {
		return 0, 0, 0, fmt.Errorf("empty data slice")
	}

	// Check for NaN values
	for _, v := range data {
		if math.IsNaN(v) {
			return 0, 0, 0, fmt.Errorf("data contains NaN values")
		}
	}

	var cq1, cq2, cq3 C.double
	status := C.fc_stats_quartiles_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		&cq1,
		&cq2,
		&cq3,
	)

	if status != C.FC_OK {
		return 0, 0, 0, fmt.Errorf("quartiles computation failed with status %d", status)
	}

	return float64(cq1), float64(cq2), float64(cq3), nil
}

// QuantileNonDestructive computes a quantile without modifying the input slice.
// This function creates a copy of the data before computation.
//
// Parameters:
//   - data: Input data slice (will not be modified)
//   - q: Quantile to compute (0.0 to 1.0)
//
// Returns the quantile value and any error encountered.
//
// Note: This function is less efficient than Quantile due to the copy operation.
func QuantileNonDestructive(data []float64, q float64) (float64, error) {
	dataCopy := make([]float64, len(data))
	copy(dataCopy, data)
	return Quantile(dataCopy, q)
}

// MedianNonDestructive computes the median without modifying the input slice.
// This function creates a copy of the data before computation.
//
// Parameters:
//   - data: Input data slice (will not be modified)
//
// Returns the median value and any error encountered.
//
// Note: This function is less efficient than Median due to the copy operation.
func MedianNonDestructive(data []float64) (float64, error) {
	dataCopy := make([]float64, len(data))
	copy(dataCopy, data)
	return Median(dataCopy)
}

// QuartilesNonDestructive computes quartiles without modifying the input slice.
// This function creates a copy of the data before computation.
//
// Parameters:
//   - data: Input data slice (will not be modified)
//
// Returns Q1, Q2, Q3, and any error encountered.
//
// Note: This function is less efficient than Quartiles due to the copy operation.
func QuartilesNonDestructive(data []float64) (q1, q2, q3 float64, err error) {
	dataCopy := make([]float64, len(data))
	copy(dataCopy, data)
	return Quartiles(dataCopy)
}
