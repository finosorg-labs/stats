package stats

/*
#include "kahan_sum.h"
*/
import "C"
import "unsafe"

// KahanSum computes the sum of a slice using Kahan compensated summation.
// This provides higher numerical accuracy than naive summation, especially
// when summing many numbers or when magnitudes vary greatly.
//
// Time Complexity: O(n)
// Space Complexity: O(1)
//
// Example:
//
//	data := []float64{1e10, 1.0, -1e10, 1.0}
//	sum := stats.KahanSum(data)
//	// sum = 2.0 (accurate, naive sum might give 0.0)
func KahanSum(data []float64) float64 {
	if len(data) == 0 {
		return 0.0
	}

	return float64(C.fc_stat_kahan_sum_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
	))
}

// KahanSumF32 computes the sum of a float32 slice using Kahan compensated summation.
//
// Time Complexity: O(n)
// Space Complexity: O(1)
func KahanSumF32(data []float32) float32 {
	if len(data) == 0 {
		return 0.0
	}

	return float32(C.fc_stat_kahan_sum_f32(
		(*C.float)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
	))
}

// KahanState maintains running sum and compensation for incremental Kahan summation.
// Useful when data arrives in chunks or streams.
//
// Example:
//
//	state := stats.NewKahanState()
//	state.Add(1.0)
//	state.Add(2.0)
//	state.AddBatch([]float64{3.0, 4.0, 5.0})
//	sum := state.Sum() // 15.0
type KahanState struct {
	state C.fc_kahan_state_t
}

// NewKahanState creates and initializes a new KahanState.
func NewKahanState() *KahanState {
	ks := &KahanState{}
	C.fc_stat_kahan_init(&ks.state)
	return ks
}

// Add adds a single value to the Kahan sum.
func (ks *KahanState) Add(value float64) {
	C.fc_stat_kahan_add(&ks.state, C.double(value))
}

// AddBatch adds multiple values to the Kahan sum.
func (ks *KahanState) AddBatch(data []float64) {
	if len(data) == 0 {
		return
	}

	C.fc_stat_kahan_add_batch(
		&ks.state,
		(*C.double)(unsafe.Pointer(&data[0])),
		C.size_t(len(data)),
	)
}

// Sum returns the current sum value.
func (ks *KahanState) Sum() float64 {
	return float64(C.fc_stat_kahan_get_sum(&ks.state))
}

// Reset resets the state to zero.
func (ks *KahanState) Reset() {
	C.fc_stat_kahan_init(&ks.state)
}
