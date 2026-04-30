package stats

/*
#include "stats.h"
#include "welford.h"
#include <stdlib.h>
*/
import "C"
import (
	"unsafe"
)

// WelfordState represents the state of Welford's online statistics algorithm
type WelfordState struct {
	state C.fc_welford_state_t
}

// NewWelford creates and initializes a new Welford state
func NewWelford() (*WelfordState, error) {
	w := &WelfordState{}
	status := C.fc_welford_init(&w.state)
	if err := statusToError(status); err != nil {
		return nil, err
	}
	return w, nil
}

// Update adds a single value to the running statistics
func (w *WelfordState) Update(value float64) error {
	status := C.fc_welford_update(&w.state, C.double(value))
	return statusToError(status)
}

// UpdateBatch adds multiple values to the running statistics
func (w *WelfordState) UpdateBatch(values []float64) error {
	if len(values) == 0 {
		return ErrInvalidArg
	}

	status := C.fc_welford_update_batch(
		&w.state,
		(*C.double)(unsafe.Pointer(&values[0])),
		C.size_t(len(values)),
	)
	return statusToError(status)
}

// Mean returns the current mean
func (w *WelfordState) Mean() (float64, error) {
	var mean C.double
	status := C.fc_welford_mean(&w.state, &mean)
	if err := statusToError(status); err != nil {
		return 0, err
	}
	return float64(mean), nil
}

// Variance returns the current variance
// If sample is true, computes sample variance (divides by n-1)
// If sample is false, computes population variance (divides by n)
func (w *WelfordState) Variance(sample bool) (float64, error) {
	var variance C.double
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_welford_variance(&w.state, &variance, sampleFlag)
	if err := statusToError(status); err != nil {
		return 0, err
	}
	return float64(variance), nil
}

// StdDev returns the current standard deviation
func (w *WelfordState) StdDev(sample bool) (float64, error) {
	var stddev C.double
	sampleFlag := C.int(0)
	if sample {
		sampleFlag = C.int(1)
	}

	status := C.fc_welford_stddev(&w.state, &stddev, sampleFlag)
	if err := statusToError(status); err != nil {
		return 0, err
	}
	return float64(stddev), nil
}

// Count returns the number of values processed
func (w *WelfordState) Count() (int, error) {
	var count C.size_t
	status := C.fc_welford_count(&w.state, &count)
	if err := statusToError(status); err != nil {
		return 0, err
	}
	return int(count), nil
}

// Merge combines statistics from another Welford state
// After merge, w contains the combined statistics
func (w *WelfordState) Merge(other *WelfordState) error {
	if other == nil {
		return ErrInvalidArg
	}

	status := C.fc_welford_merge(&w.state, &other.state)
	return statusToError(status)
}

// Reset reinitializes the state to empty
func (w *WelfordState) Reset() error {
	status := C.fc_welford_init(&w.state)
	return statusToError(status)
}

// UpdateStreams updates multiple independent data streams in parallel using SIMD optimization.
// This is highly efficient for updating many independent statistics simultaneously.
//
// Each element in states corresponds to one data stream, and each element in values
// provides the new value for that stream.
//
// SIMD acceleration:
//   - SSE4.2: processes 2 streams in parallel
//   - AVX2: processes 4 streams in parallel
//   - AVX-512: processes 8 streams in parallel
//
// Use case: Real-time statistics for multiple assets (e.g., 5000 stocks).
func UpdateStreams(states []*WelfordState, values []float64) error {
	if len(states) != len(values) {
		return ErrInvalidArg
	}
	if len(states) == 0 {
		return nil
	}

	// Create array of C states
	cStates := make([]C.fc_welford_state_t, len(states))
	for i, s := range states {
		if s == nil {
			return ErrInvalidArg
		}
		cStates[i] = s.state
	}

	// Call C function
	status := C.fc_welford_update_streams(
		(*C.fc_welford_state_t)(unsafe.Pointer(&cStates[0])),
		(*C.double)(unsafe.Pointer(&values[0])),
		C.size_t(len(values)),
	)

	// Copy states back
	for i := range states {
		states[i].state = cStates[i]
	}

	return statusToError(status)
}

