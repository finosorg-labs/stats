package stats

/*
#include "include/moments.h"
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
)

// MomentsState represents the state for online higher-order moments computation
type MomentsState struct {
	state C.fc_moments_state_t
}

// NewMomentsState creates and initializes a new moments state
func NewMomentsState() *MomentsState {
	m := &MomentsState{}
	C.fc_stats_moments_init(&m.state)
	return m
}

// Update adds a single value to the moments computation
func (m *MomentsState) Update(value float64) {
	C.fc_stats_moments_update(&m.state, C.double(value))
}

// UpdateBatch adds multiple values to the moments computation
func (m *MomentsState) UpdateBatch(values []float64) {
	if len(values) == 0 {
		return
	}
	C.fc_stats_moments_update_batch(&m.state, (*C.double)(&values[0]), C.size_t(len(values)))
}

// Count returns the number of samples processed
func (m *MomentsState) Count() int {
	return int(m.state.n)
}

// Mean returns the current mean
func (m *MomentsState) Mean() float64 {
	return float64(m.state.mean)
}

// Skewness computes the sample skewness
// Returns NaN if fewer than 3 samples
func (m *MomentsState) Skewness() float64 {
	return float64(C.fc_stats_moments_skewness(&m.state))
}

// Kurtosis computes the sample kurtosis
// Returns NaN if fewer than 4 samples
func (m *MomentsState) Kurtosis() float64 {
	return float64(C.fc_stats_moments_kurtosis(&m.state))
}

// ExcessKurtosis computes the sample excess kurtosis (kurtosis - 3)
// Returns NaN if fewer than 4 samples
func (m *MomentsState) ExcessKurtosis() float64 {
	return float64(C.fc_stats_moments_excess_kurtosis(&m.state))
}

// Skewness computes skewness for multiple groups in batch
// data is organized as n_groups × group_size
// Returns error if group_size < 3
func Skewness(data []float64, nGroups, groupSize int) ([]float64, error) {
	if nGroups <= 0 || groupSize < 3 {
		return nil, fmt.Errorf("invalid dimensions: nGroups=%d, groupSize=%d (groupSize must be >= 3)", nGroups, groupSize)
	}

	if len(data) < nGroups*groupSize {
		return nil, fmt.Errorf("insufficient data: got %d, need %d", len(data), nGroups*groupSize)
	}

	out := make([]float64, nGroups)
	status := C.fc_stats_skewness_f64(
		(*C.double)(&out[0]),
		(*C.double)(&data[0]),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("fc_stats_skewness_f64 failed with status %d", status)
	}

	return out, nil
}

// Kurtosis computes kurtosis for multiple groups in batch
// data is organized as n_groups × group_size
// Returns error if group_size < 4
func Kurtosis(data []float64, nGroups, groupSize int) ([]float64, error) {
	if nGroups <= 0 || groupSize < 4 {
		return nil, fmt.Errorf("invalid dimensions: nGroups=%d, groupSize=%d (groupSize must be >= 4)", nGroups, groupSize)
	}

	if len(data) < nGroups*groupSize {
		return nil, fmt.Errorf("insufficient data: got %d, need %d", len(data), nGroups*groupSize)
	}

	out := make([]float64, nGroups)
	status := C.fc_stats_kurtosis_f64(
		(*C.double)(&out[0]),
		(*C.double)(&data[0]),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("fc_stats_kurtosis_f64 failed with status %d", status)
	}

	return out, nil
}

// ExcessKurtosis computes excess kurtosis for multiple groups in batch
// data is organized as n_groups × group_size
// Returns error if group_size < 4
func ExcessKurtosis(data []float64, nGroups, groupSize int) ([]float64, error) {
	if nGroups <= 0 || groupSize < 4 {
		return nil, fmt.Errorf("invalid dimensions: nGroups=%d, groupSize=%d (groupSize must be >= 4)", nGroups, groupSize)
	}

	if len(data) < nGroups*groupSize {
		return nil, fmt.Errorf("insufficient data: got %d, need %d", len(data), nGroups*groupSize)
	}

	out := make([]float64, nGroups)
	status := C.fc_stats_excess_kurtosis_f64(
		(*C.double)(&out[0]),
		(*C.double)(&data[0]),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)

	if status != C.FC_OK {
		return nil, fmt.Errorf("fc_stats_excess_kurtosis_f64 failed with status %d", status)
	}

	return out, nil
}
