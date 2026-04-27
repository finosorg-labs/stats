package stats

/*
#include "stats.h"
#include "mean_var.h"
#include <stdlib.h>
*/
import "C"
import (
	"errors"
	"unsafe"
)

// Error definitions
var (
	ErrInvalidArg        = errors.New("invalid argument")
	ErrNaNInput          = errors.New("input contains NaN")
	ErrInsufficientData  = errors.New("insufficient data for sample variance")
)

// statusToError converts C status code to Go error
func statusToError(status C.fc_status_t) error {
	switch status {
	case C.FC_OK:
		return nil
	case C.FC_ERR_INVALID_ARG:
		return ErrInvalidArg
	case C.FC_ERR_NAN_INPUT:
		return ErrNaNInput
	default:
		return errors.New("unknown error")
	}
}

// Mean computes the mean of a float64 slice
func Mean(data []float64) (float64, error) {
	if len(data) == 0 {
		return 0, ErrInvalidArg
	}

	var mean C.double
	status := C.fc_stats_mean_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		&mean,
	)

	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(mean), nil
}

// Variance computes the variance of a float64 slice
// If sample is true, computes sample variance (divides by n-1)
// If sample is false, computes population variance (divides by n)
func Variance(data []float64, sample bool) (float64, error) {
	if sample && len(data) < 2 {
		return 0, ErrInsufficientData
	}
	if !sample && len(data) == 0 {
		return 0, ErrInvalidArg
	}

	var variance C.double
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stats_variance_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		&variance,
		sampleFlag,
	)

	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(variance), nil
}

// MeanVariance computes both mean and variance simultaneously
// More efficient than calling Mean and Variance separately
func MeanVariance(data []float64, sample bool) (mean, variance float64, err error) {
	if sample && len(data) < 2 {
		return 0, 0, ErrInsufficientData
	}
	if !sample && len(data) == 0 {
		return 0, 0, ErrInvalidArg
	}

	var cMean, cVariance C.double
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stats_mean_variance_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		&cMean,
		&cVariance,
		sampleFlag,
	)

	if err := statusToError(status); err != nil {
		return 0, 0, err
	}

	return float64(cMean), float64(cVariance), nil
}

// StdDev computes the standard deviation of a float64 slice
func StdDev(data []float64, sample bool) (float64, error) {
	if sample && len(data) < 2 {
		return 0, ErrInsufficientData
	}
	if !sample && len(data) == 0 {
		return 0, ErrInvalidArg
	}

	var stddev C.double
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stats_stddev_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		&stddev,
		sampleFlag,
	)

	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(stddev), nil
}

// MeanBatch computes mean for multiple arrays
// All arrays must have the same length
func MeanBatch(data [][]float64) ([]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}

	n := len(data[0])
	for i := 1; i < len(data); i++ {
		if len(data[i]) != n {
			return nil, ErrInvalidArg
		}
	}

	// Flatten data into a single contiguous array
	flatData := make([]float64, len(data)*n)
	for i := range data {
		copy(flatData[i*n:(i+1)*n], data[i])
	}

	// Allocate C memory for pointer array
	cDataPtrs := C.malloc(C.size_t(len(data)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	defer C.free(cDataPtrs)

	// Create array of pointers pointing into flatData
	ptrArray := (*[1 << 30]*C.double)(cDataPtrs)[:len(data):len(data)]
	for i := range data {
		ptrArray[i] = (*C.double)(unsafe.Pointer(&flatData[i*n]))
	}

	means := make([]float64, len(data))

	status := C.fc_stats_mean_batch_f64(
		(**C.double)(cDataPtrs),
		C.size_t(n),
		C.size_t(len(data)),
		(*C.double)(unsafe.Pointer(&means[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	return means, nil
}

// VarianceBatch computes variance for multiple arrays
// All arrays must have the same length
func VarianceBatch(data [][]float64, sample bool) ([]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}

	n := len(data[0])
	if sample && n < 2 {
		return nil, ErrInsufficientData
	}

	for i := 1; i < len(data); i++ {
		if len(data[i]) != n {
			return nil, ErrInvalidArg
		}
	}

	// Flatten data into a single contiguous array
	flatData := make([]float64, len(data)*n)
	for i := range data {
		copy(flatData[i*n:(i+1)*n], data[i])
	}

	// Allocate C memory for pointer array
	cDataPtrs := C.malloc(C.size_t(len(data)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	defer C.free(cDataPtrs)

	// Create array of pointers pointing into flatData
	ptrArray := (*[1 << 30]*C.double)(cDataPtrs)[:len(data):len(data)]
	for i := range data {
		ptrArray[i] = (*C.double)(unsafe.Pointer(&flatData[i*n]))
	}

	variances := make([]float64, len(data))
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stats_variance_batch_f64(
		(**C.double)(cDataPtrs),
		C.size_t(n),
		C.size_t(len(data)),
		(*C.double)(unsafe.Pointer(&variances[0])),
		sampleFlag,
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	return variances, nil
}

// MeanVarianceBatch computes both mean and variance for multiple arrays
// All arrays must have the same length
func MeanVarianceBatch(data [][]float64, sample bool) (means, variances []float64, err error) {
	if len(data) == 0 {
		return nil, nil, ErrInvalidArg
	}

	n := len(data[0])
	if sample && n < 2 {
		return nil, nil, ErrInsufficientData
	}

	for i := 1; i < len(data); i++ {
		if len(data[i]) != n {
			return nil, nil, ErrInvalidArg
		}
	}

	// Flatten data into a single contiguous array
	flatData := make([]float64, len(data)*n)
	for i := range data {
		copy(flatData[i*n:(i+1)*n], data[i])
	}

	// Allocate C memory for pointer array
	cDataPtrs := C.malloc(C.size_t(len(data)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	defer C.free(cDataPtrs)

	// Create array of pointers pointing into flatData
	ptrArray := (*[1 << 30]*C.double)(cDataPtrs)[:len(data):len(data)]
	for i := range data {
		ptrArray[i] = (*C.double)(unsafe.Pointer(&flatData[i*n]))
	}

	means = make([]float64, len(data))
	variances = make([]float64, len(data))
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stats_mean_variance_batch_f64(
		(**C.double)(cDataPtrs),
		C.size_t(n),
		C.size_t(len(data)),
		(*C.double)(unsafe.Pointer(&means[0])),
		(*C.double)(unsafe.Pointer(&variances[0])),
		sampleFlag,
	)

	if err := statusToError(status); err != nil {
		return nil, nil, err
	}

	return means, variances, nil
}

