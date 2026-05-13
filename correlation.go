package stats

/*
#include "stats.h"
#include "correlation.h"
#include <stdlib.h>
*/
import "C"
import (
	"unsafe"
)

// Correlation computes the Pearson correlation coefficient between two slices.
//
// The Pearson correlation coefficient measures the linear relationship between
// two variables. It ranges from -1 (perfect negative correlation) to +1 (perfect
// positive correlation), with 0 indicating no linear correlation.
//
// Parameters:
//   - x: First data slice (must have length >= 2)
//   - y: Second data slice (must have same length as x)
//
// Returns:
//   - Correlation coefficient in range [-1, 1]
//   - Error if inputs are invalid or contain NaN
//
// Performance: ~5x faster than pure Go implementation for n >= 1000
func Correlation(x, y []float64) (float64, error) {
	if len(x) != len(y) {
		return 0, ErrInvalidArg
	}
	if len(x) < 2 {
		return 0, ErrInvalidArg
	}

	var corr C.double
	status := C.fc_stats_pearson_correlation_f64(
		(*C.double)(unsafe.Pointer(&x[0])),
		(*C.double)(unsafe.Pointer(&y[0])),
		C.size_t(len(x)),
		&corr,
	)

	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(corr), nil
}

// CorrelationMatrix computes the Pearson correlation matrix for multiple variables.
//
// Input data should be organized as a 2D slice where each inner slice represents
// one variable (column). All variables must have the same number of observations.
//
// The output is a symmetric matrix where element [i][j] represents the correlation
// between variable i and variable j. Diagonal elements are always 1.0.
//
// Parameters:
//   - data: 2D slice where data[i] is the i-th variable (all must have same length)
//
// Returns:
//   - Correlation matrix as 2D slice (n_vars × n_vars)
//   - Error if inputs are invalid or contain NaN
//
// Performance target: 500 variables × 250 samples < 50ms (vs Go > 250ms)
//
// Example:
//
//	data := [][]float64{
//	    {1.0, 2.0, 3.0, 4.0},  // variable 0
//	    {2.0, 4.0, 6.0, 8.0},  // variable 1
//	    {5.0, 4.0, 3.0, 2.0},  // variable 2
//	}
//	corrMatrix, err := CorrelationMatrix(data)
//	// corrMatrix[0][1] = correlation between var 0 and var 1
func CorrelationMatrix(data [][]float64) ([][]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}

	nVars := len(data)
	nSamples := len(data[0])

	if nSamples < 2 {
		return nil, ErrInvalidArg
	}

	// Validate all variables have same length
	for i := 1; i < nVars; i++ {
		if len(data[i]) != nSamples {
			return nil, ErrInvalidArg
		}
	}

	// Convert to column-major layout for C
	flatData := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples; i++ {
		for j := 0; j < nVars; j++ {
			flatData[i*nVars+j] = data[j][i]
		}
	}

	// Allocate output matrix
	corrMatrix := make([]float64, nVars*nVars)

	status := C.fc_stats_pearson_correlation_matrix_f64(
		(*C.double)(unsafe.Pointer(&flatData[0])),
		C.size_t(nSamples),
		C.size_t(nVars),
		(*C.double)(unsafe.Pointer(&corrMatrix[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	// Convert flat matrix to 2D slice
	result := make([][]float64, nVars)
	for i := 0; i < nVars; i++ {
		result[i] = make([]float64, nVars)
		for j := 0; j < nVars; j++ {
			result[i][j] = corrMatrix[i*nVars+j]
		}
	}

	return result, nil
}

// CorrelationMatrixFlat computes the correlation matrix and returns it as a flat slice.
//
// This is a more efficient version of CorrelationMatrix that avoids the overhead
// of converting to a 2D slice. The output is in row-major order.
//
// Parameters:
//   - data: 2D slice where data[i] is the i-th variable
//
// Returns:
//   - Flat correlation matrix in row-major order (length = n_vars²)
//   - Number of variables
//   - Error if inputs are invalid
//
// Access pattern: corrMatrix[i*nVars + j] = correlation(var_i, var_j)
func CorrelationMatrixFlat(data [][]float64) ([]float64, int, error) {
	if len(data) == 0 {
		return nil, 0, ErrInvalidArg
	}

	nVars := len(data)
	nSamples := len(data[0])

	if nSamples < 2 {
		return nil, 0, ErrInvalidArg
	}

	// Validate all variables have same length
	for i := 1; i < nVars; i++ {
		if len(data[i]) != nSamples {
			return nil, 0, ErrInvalidArg
		}
	}

	// Convert to column-major layout
	flatData := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples; i++ {
		for j := 0; j < nVars; j++ {
			flatData[i*nVars+j] = data[j][i]
		}
	}

	// Allocate output matrix
	corrMatrix := make([]float64, nVars*nVars)

	status := C.fc_stats_pearson_correlation_matrix_f64(
		(*C.double)(unsafe.Pointer(&flatData[0])),
		C.size_t(nSamples),
		C.size_t(nVars),
		(*C.double)(unsafe.Pointer(&corrMatrix[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, 0, err
	}

	return corrMatrix, nVars, nil
}

// CorrelationMatrixPrecomputed computes the correlation matrix using pre-computed
// means and standard deviations.
//
// This is more efficient when statistics are already available or need to be cached.
//
// Parameters:
//   - data: 2D slice where data[i] is the i-th variable
//   - means: Pre-computed mean for each variable
//   - stddevs: Pre-computed standard deviation for each variable
//
// Returns:
//   - Correlation matrix as 2D slice
//   - Error if inputs are invalid
func CorrelationMatrixPrecomputed(data [][]float64, means, stddevs []float64) ([][]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}

	nVars := len(data)
	nSamples := len(data[0])

	if nSamples < 2 {
		return nil, ErrInvalidArg
	}

	if len(means) != nVars || len(stddevs) != nVars {
		return nil, ErrInvalidArg
	}

	// Validate all variables have same length
	for i := 1; i < nVars; i++ {
		if len(data[i]) != nSamples {
			return nil, ErrInvalidArg
		}
	}

	// Convert to column-major layout
	flatData := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples; i++ {
		for j := 0; j < nVars; j++ {
			flatData[i*nVars+j] = data[j][i]
		}
	}

	// Allocate output matrix
	corrMatrix := make([]float64, nVars*nVars)

	status := C.fc_stats_pearson_correlation_matrix_precomputed_f64(
		(*C.double)(unsafe.Pointer(&flatData[0])),
		C.size_t(nSamples),
		C.size_t(nVars),
		(*C.double)(unsafe.Pointer(&means[0])),
		(*C.double)(unsafe.Pointer(&stddevs[0])),
		(*C.double)(unsafe.Pointer(&corrMatrix[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	// Convert flat matrix to 2D slice
	result := make([][]float64, nVars)
	for i := 0; i < nVars; i++ {
		result[i] = make([]float64, nVars)
		for j := 0; j < nVars; j++ {
			result[i][j] = corrMatrix[i*nVars+j]
		}
	}

	return result, nil
}
