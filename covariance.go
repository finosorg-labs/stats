package stats

/*
#include "covariance.h"
#include "modules/platform/include/error.h"
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// Covariance computes the covariance matrix for multiple variables.
//
// Parameters:
//   - data: Input data matrix (nSamples × nVars), row-major order
//   - nSamples: Number of samples (rows)
//   - nVars: Number of variables (columns)
//   - sampleCov: If true, use sample covariance (divide by n-1); if false, use population covariance (divide by n)
//
// Returns:
//   - Covariance matrix (nVars × nVars), row-major order
//   - Error if any
//
// The covariance matrix is symmetric and positive semi-definite.
// Time complexity: O(nVars² × nSamples)
func Covariance(data []float64, nSamples, nVars int, sampleCov bool) ([]float64, error) {
	if len(data) != nSamples*nVars {
		return nil, fmt.Errorf("data length mismatch: expected %d, got %d", nSamples*nVars, len(data))
	}

	if nSamples == 0 || nVars == 0 {
		return nil, fmt.Errorf("invalid dimensions")
	}

	cov := make([]float64, nVars*nVars)

	sampleCovInt := C.int(0)
	if sampleCov {
		sampleCovInt = C.int(1)
	}

	status := C.fc_stats_covariance_f64(
		(*C.double)(unsafe.Pointer(&cov[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(nSamples),
		C.size_t(nVars),
		sampleCovInt,
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("covariance computation failed: %s", C.GoString(C.fc_status_string(status)))
	}

	return cov, nil
}

// Correlation computes the correlation coefficient matrix from a covariance matrix.
//
// Parameters:
//   - cov: Input covariance matrix (nVars × nVars), row-major order
//   - nVars: Number of variables
//
// Returns:
//   - Correlation matrix (nVars × nVars), row-major order
//   - Error if any
//
// The correlation matrix has 1.0 on the diagonal and values in [-1, 1] elsewhere.
// Time complexity: O(nVars²)
func Correlation(cov []float64, nVars int) ([]float64, error) {
	if len(cov) != nVars*nVars {
		return nil, fmt.Errorf("covariance matrix size mismatch: expected %d, got %d", nVars*nVars, len(cov))
	}

	if nVars == 0 {
		return nil, fmt.Errorf("invalid dimensions")
	}

	corr := make([]float64, nVars*nVars)

	status := C.fc_stats_correlation_f64(
		(*C.double)(unsafe.Pointer(&corr[0])),
		(*C.double)(unsafe.Pointer(&cov[0])),
		C.size_t(nVars),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("correlation computation failed: %s", C.GoString(C.fc_status_string(status)))
	}

	return corr, nil
}

// CovarianceCorrelation computes both covariance and correlation matrices in one pass.
//
// This is more efficient than calling Covariance and Correlation separately.
//
// Parameters:
//   - data: Input data matrix (nSamples × nVars), row-major order
//   - nSamples: Number of samples (rows)
//   - nVars: Number of variables (columns)
//   - sampleCov: If true, use sample covariance; if false, use population covariance
//
// Returns:
//   - Covariance matrix (nVars × nVars), row-major order
//   - Correlation matrix (nVars × nVars), row-major order
//   - Error if any
//
// Time complexity: O(nVars² × nSamples)
func CovarianceCorrelation(data []float64, nSamples, nVars int, sampleCov bool) ([]float64, []float64, error) {
	if len(data) != nSamples*nVars {
		return nil, nil, fmt.Errorf("data length mismatch: expected %d, got %d", nSamples*nVars, len(data))
	}

	if nSamples == 0 || nVars == 0 {
		return nil, nil, fmt.Errorf("invalid dimensions")
	}

	cov := make([]float64, nVars*nVars)
	corr := make([]float64, nVars*nVars)

	sampleCovInt := C.int(0)
	if sampleCov {
		sampleCovInt = C.int(1)
	}

	status := C.fc_stats_covariance_correlation_f64(
		(*C.double)(unsafe.Pointer(&cov[0])),
		(*C.double)(unsafe.Pointer(&corr[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(nSamples),
		C.size_t(nVars),
		sampleCovInt,
	)

	if status != C.FC_OK {
		return nil, nil, fmt.Errorf("covariance/correlation computation failed: %s", C.GoString(C.fc_status_string(status)))
	}

	return cov, corr, nil
}

// Spearman computes the Spearman rank correlation coefficient matrix.
//
// The Spearman correlation is a non-parametric measure of monotonic association,
// computed as the Pearson correlation of the ranked data. It measures how well
// the relationship between two variables can be described by a monotonic function.
//
// Parameters:
//   - data: Input data matrix (nSamples × nVars), row-major order
//   - nSamples: Number of samples (rows)
//   - nVars: Number of variables (columns)
//
// Returns:
//   - Spearman correlation matrix (nVars × nVars), row-major order
//   - Error if any
//
// The correlation matrix has 1.0 on the diagonal and values in [-1, 1] elsewhere.
// Ties are handled using the average rank method.
// Time complexity: O(nVars × nSamples × log(nSamples))
func Spearman(data []float64, nSamples, nVars int) ([]float64, error) {
	if len(data) != nSamples*nVars {
		return nil, fmt.Errorf("data length mismatch: expected %d, got %d", nSamples*nVars, len(data))
	}

	if nSamples < 2 || nVars == 0 {
		return nil, fmt.Errorf("invalid dimensions: need at least 2 samples")
	}

	corr := make([]float64, nVars*nVars)

	status := C.fc_stats_spearman_f64(
		(*C.double)(unsafe.Pointer(&corr[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(nSamples),
		C.size_t(nVars),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("Spearman correlation computation failed: %s", C.GoString(C.fc_status_string(status)))
	}

	return corr, nil
}
