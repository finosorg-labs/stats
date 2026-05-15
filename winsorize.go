package stats

/*
#include "stats.h"
#include "winsorize.h"
#include <stdlib.h>
*/
import "C"
import (
	"unsafe"
)

// Winsorize replaces extreme values with percentile limits
//
// Replaces values below the lower percentile with the lower percentile value,
// and values above the upper percentile with the upper percentile value.
//
// Parameters:
//   - data: Input slice
//   - lowerPct: Lower percentile (0.0 to 1.0, must be < upperPct)
//   - upperPct: Upper percentile (0.0 to 1.0, must be > lowerPct)
//
// Returns:
//   - Winsorized slice
//   - Error if any
//
// Example:
//   // 5% symmetric winsorization
//   result, err := Winsorize(data, 0.05, 0.95)
func Winsorize(data []float64, lowerPct, upperPct float64) ([]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}

	if lowerPct < 0.0 || lowerPct > 1.0 || upperPct < 0.0 || upperPct > 1.0 {
		return nil, ErrInvalidArg
	}

	if lowerPct >= upperPct {
		return nil, ErrInvalidArg
	}

	output := make([]float64, len(data))

	status := C.fc_stats_winsorize_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.double(lowerPct),
		C.double(upperPct),
		(*C.double)(unsafe.Pointer(&output[0])),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	return output, nil
}

// WinsorizeInPlace performs in-place winsorization
//
// Same as Winsorize but modifies the input slice in place.
// More memory efficient as it doesn't allocate output buffer.
//
// Parameters:
//   - data: Input/output slice (will be modified)
//   - lowerPct: Lower percentile (0.0 to 1.0, must be < upperPct)
//   - upperPct: Upper percentile (0.0 to 1.0, must be > lowerPct)
//
// Returns:
//   - Error if any
//
// Example:
//   err := WinsorizeInPlace(data, 0.05, 0.95)
func WinsorizeInPlace(data []float64, lowerPct, upperPct float64) error {
	if len(data) == 0 {
		return ErrInvalidArg
	}

	if lowerPct < 0.0 || lowerPct > 1.0 || upperPct < 0.0 || upperPct > 1.0 {
		return ErrInvalidArg
	}

	if lowerPct >= upperPct {
		return ErrInvalidArg
	}

	status := C.fc_stats_winsorize_inplace_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
		C.double(lowerPct),
		C.double(upperPct),
	)

	return statusToError(status)
}

// WinsorizeBatch performs batch winsorization on multiple slices
//
// Processes multiple independent datasets in a single call.
// Each slice is winsorized independently with the same percentile limits.
//
// Parameters:
//   - data: Slice of input slices (all must have the same length)
//   - lowerPct: Lower percentile (0.0 to 1.0, must be < upperPct)
//   - upperPct: Upper percentile (0.0 to 1.0, must be > lowerPct)
//
// Returns:
//   - Slice of winsorized slices
//   - Error if any
//
// Example:
//   results, err := WinsorizeBatch([][]float64{data1, data2, data3}, 0.05, 0.95)
func WinsorizeBatch(data [][]float64, lowerPct, upperPct float64) ([][]float64, error) {
	if len(data) == 0 {
		return nil, ErrInvalidArg
	}

	n := len(data[0])
	if n == 0 {
		return nil, ErrInvalidArg
	}

	// Verify all slices have the same length
	for i := 1; i < len(data); i++ {
		if len(data[i]) != n {
			return nil, ErrInvalidArg
		}
	}

	if lowerPct < 0.0 || lowerPct > 1.0 || upperPct < 0.0 || upperPct > 1.0 {
		return nil, ErrInvalidArg
	}

	if lowerPct >= upperPct {
		return nil, ErrInvalidArg
	}

	// Allocate C memory for pointer arrays
	cData := C.malloc(C.size_t(len(data)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	if cData == nil {
		return nil, ErrInvalidArg
	}
	defer C.free(cData)

	cOutput := C.malloc(C.size_t(len(data)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	if cOutput == nil {
		return nil, ErrInvalidArg
	}
	defer C.free(cOutput)

	// Convert to pointer arrays
	cDataSlice := (*[1 << 30]*C.double)(cData)[:len(data):len(data)]
	cOutputSlice := (*[1 << 30]*C.double)(cOutput)[:len(data):len(data)]

	for i := range data {
		cDataSlice[i] = (*C.double)(unsafe.Pointer(&data[i][0]))
	}

	// Prepare output slices
	output := make([][]float64, len(data))
	for i := range output {
		output[i] = make([]float64, n)
		cOutputSlice[i] = (*C.double)(unsafe.Pointer(&output[i][0]))
	}

	status := C.fc_stats_winsorize_batch_f64(
		(**C.double)(cData),
		C.size_t(n),
		C.size_t(len(data)),
		C.double(lowerPct),
		C.double(upperPct),
		(**C.double)(cOutput),
	)

	if err := statusToError(status); err != nil {
		return nil, err
	}

	return output, nil
}
