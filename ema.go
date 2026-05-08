package stats

/*
#include "stats.h"
#include "ema.h"
#include <stdlib.h>
*/
import "C"
import (
	"unsafe"
)

// EMA computes the exponential moving average of a time series.
//
// The EMA is calculated using the formula:
//
//	EMA[0] = data[0]
//	EMA[i] = alpha * data[i] + (1 - alpha) * EMA[i-1]
//
// where alpha is the smoothing factor (0 < alpha <= 1).
// Common choice: alpha = 2 / (period + 1)
//
// Parameters:
//   - data: Input time series data
//   - alpha: Smoothing factor (must be in (0, 1])
//
// Returns:
//   - ema: Output EMA values (same length as input)
//   - error: Error if any
//
// Example:
//
//	data := []float64{100, 102, 101, 103, 105}
//	alpha := 2.0 / (5 + 1)  // 5-period EMA
//	ema, err := stats.EMA(data, alpha)
func EMA(data []float64, alpha float64) ([]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}
	if alpha <= 0 || alpha > 1 {
		return nil, ErrInvalidArg
	}

	ema := make([]float64, len(data))

	status := C.fc_stats_ema_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.double(alpha),
		(*C.double)(unsafe.Pointer(&ema[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	return ema, nil
}

// EMAWithInit computes the EMA with a custom initial value.
//
// This allows specifying the initial EMA value instead of using data[0].
// Useful when continuing an EMA calculation from a previous state.
//
// Parameters:
//   - data: Input time series data
//   - alpha: Smoothing factor (must be in (0, 1])
//   - initValue: Initial EMA value
//
// Returns:
//   - ema: Output EMA values (same length as input)
//   - error: Error if any
func EMAWithInit(data []float64, alpha float64, initValue float64) ([]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}
	if alpha <= 0 || alpha > 1 {
		return nil, ErrInvalidArg
	}

	ema := make([]float64, len(data))

	status := C.fc_stats_ema_f64_init(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.double(alpha),
		C.double(initValue),
		(*C.double)(unsafe.Pointer(&ema[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	return ema, nil
}

// EMAInPlace computes the EMA in-place, modifying the input slice.
//
// This is more memory-efficient as it doesn't allocate a new slice.
//
// Parameters:
//   - data: Input time series data (will be modified)
//   - alpha: Smoothing factor (must be in (0, 1])
//
// Returns:
//   - error: Error if any
//
// Example:
//
//	data := []float64{100, 102, 101, 103, 105}
//	err := stats.EMAInPlace(data, 0.3)
//	// data now contains the EMA values
func EMAInPlace(data []float64, alpha float64) error {
	if len(data) == 0 {
		return ErrInvalidArg
	}
	if alpha <= 0 || alpha > 1 {
		return ErrInvalidArg
	}

	status := C.fc_stats_ema_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.double(alpha),
		(*C.double)(unsafe.Pointer(&data[0])),
	)

	return statusToError(status)
}

// EMABatch computes EMA for multiple time series in parallel.
//
// All series must have the same length and use the same alpha.
// The input is a flattened array in row-major order.
//
// Parameters:
//   - data: Input data (numSeries x seriesLength, row-major)
//   - numSeries: Number of time series
//   - seriesLength: Length of each series
//   - alpha: Smoothing factor (must be in (0, 1])
//
// Returns:
//   - ema: Output EMA values (numSeries x seriesLength, row-major)
//   - error: Error if any
//
// Example:
//
//	// 3 series, each with 5 elements
//	data := []float64{
//	    1, 2, 3, 4, 5,      // Series 1
//	    10, 20, 30, 40, 50, // Series 2
//	    100, 90, 80, 70, 60, // Series 3
//	}
//	ema, err := stats.EMABatch(data, 3, 5, 0.4)
func EMABatch(data []float64, numSeries, seriesLength int, alpha float64) ([]float64, error) {
	if len(data) == 0 || numSeries == 0 || seriesLength == 0 {
		return nil, ErrInvalidArg
	}
	if len(data) != numSeries*seriesLength {
		return nil, ErrInvalidArg
	}
	if alpha <= 0 || alpha > 1 {
		return nil, ErrInvalidArg
	}

	ema := make([]float64, len(data))

	status := C.fc_stats_ema_batch_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(numSeries),
		C.size_t(seriesLength),
		C.double(alpha),
		(*C.double)(unsafe.Pointer(&ema[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	return ema, nil
}

// EMAPeriod is a convenience function that computes EMA using a period
// instead of alpha. The alpha is calculated as 2 / (period + 1).
//
// This is the most common way to specify EMA in financial applications.
//
// Parameters:
//   - data: Input time series data
//   - period: EMA period (e.g., 5 for 5-period EMA)
//
// Returns:
//   - ema: Output EMA values (same length as input)
//   - error: Error if any
//
// Example:
//
//	data := []float64{100, 102, 101, 103, 105}
//	ema, err := stats.EMAPeriod(data, 5)  // 5-period EMA
func EMAPeriod(data []float64, period int) ([]float64, error) {
	if period <= 0 {
		return nil, ErrInvalidArg
	}

	alpha := 2.0 / float64(period+1)
	return EMA(data, alpha)
}
