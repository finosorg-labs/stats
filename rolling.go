package stats

/*
#include "rolling.h"
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"unsafe"
)

// RollingMean computes the rolling window mean.
// Returns a slice of the same length as data, where output[i] contains
// the mean of elements [max(0, i-windowSize+1), i].
func RollingMean(data []float64, windowSize int) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("data cannot be empty")
	}
	if windowSize <= 0 || windowSize > len(data) {
		return nil, fmt.Errorf("invalid window size: %d (data length: %d)", windowSize, len(data))
	}

	output := make([]float64, len(data))
	status := C.fc_stats_rolling_mean_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.size_t(windowSize),
		(*C.double)(unsafe.Pointer(&output[0])),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("rolling mean failed with status %d", status)
	}

	return output, nil
}

// RollingVariance computes the rolling window variance.
// If sample is true, computes sample variance (divides by n-1).
// If sample is false, computes population variance (divides by n).
func RollingVariance(data []float64, windowSize int, sample bool) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("data cannot be empty")
	}
	if windowSize <= 0 || windowSize > len(data) {
		return nil, fmt.Errorf("invalid window size: %d (data length: %d)", windowSize, len(data))
	}

	output := make([]float64, len(data))
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stats_rolling_variance_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.size_t(windowSize),
		(*C.double)(unsafe.Pointer(&output[0])),
		sampleFlag,
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("rolling variance failed with status %d", status)
	}

	return output, nil
}

// RollingStddev computes the rolling window standard deviation.
// If sample is true, computes sample stddev (divides by n-1).
// If sample is false, computes population stddev (divides by n).
func RollingStddev(data []float64, windowSize int, sample bool) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("data cannot be empty")
	}
	if windowSize <= 0 || windowSize > len(data) {
		return nil, fmt.Errorf("invalid window size: %d (data length: %d)", windowSize, len(data))
	}

	output := make([]float64, len(data))
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_stats_rolling_stddev_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.size_t(windowSize),
		(*C.double)(unsafe.Pointer(&output[0])),
		sampleFlag,
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("rolling stddev failed with status %d", status)
	}

	return output, nil
}

// RollingSum computes the rolling window sum.
func RollingSum(data []float64, windowSize int) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("data cannot be empty")
	}
	if windowSize <= 0 || windowSize > len(data) {
		return nil, fmt.Errorf("invalid window size: %d (data length: %d)", windowSize, len(data))
	}

	output := make([]float64, len(data))
	status := C.fc_stats_rolling_sum_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.size_t(windowSize),
		(*C.double)(unsafe.Pointer(&output[0])),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("rolling sum failed with status %d", status)
	}

	return output, nil
}

// RollingMin computes the rolling window minimum.
func RollingMin(data []float64, windowSize int) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("data cannot be empty")
	}
	if windowSize <= 0 || windowSize > len(data) {
		return nil, fmt.Errorf("invalid window size: %d (data length: %d)", windowSize, len(data))
	}

	output := make([]float64, len(data))
	status := C.fc_stats_rolling_min_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.size_t(windowSize),
		(*C.double)(unsafe.Pointer(&output[0])),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("rolling min failed with status %d", status)
	}

	return output, nil
}

// RollingMax computes the rolling window maximum.
func RollingMax(data []float64, windowSize int) ([]float64, error) {
	if len(data) == 0 {
		return nil, fmt.Errorf("data cannot be empty")
	}
	if windowSize <= 0 || windowSize > len(data) {
		return nil, fmt.Errorf("invalid window size: %d (data length: %d)", windowSize, len(data))
	}

	output := make([]float64, len(data))
	status := C.fc_stats_rolling_max_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.size_t(windowSize),
		(*C.double)(unsafe.Pointer(&output[0])),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("rolling max failed with status %d", status)
	}

	return output, nil
}
