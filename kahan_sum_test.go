package stats

import (
	"math"
	"testing"
)

func TestKahanSum_Basic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	sum := KahanSum(data)
	expected := 15.0

	if math.Abs(sum-expected) > 1e-15 {
		t.Errorf("KahanSum() = %v, want %v", sum, expected)
	}
}

func TestKahanSum_Empty(t *testing.T) {
	data := []float64{}
	sum := KahanSum(data)
	expected := 0.0

	if sum != expected {
		t.Errorf("KahanSum(empty) = %v, want %v", sum, expected)
	}
}

func TestKahanSum_Single(t *testing.T) {
	data := []float64{42.0}
	sum := KahanSum(data)
	expected := 42.0

	if math.Abs(sum-expected) > 1e-15 {
		t.Errorf("KahanSum(single) = %v, want %v", sum, expected)
	}
}

func TestKahanSum_CatastrophicCancellation(t *testing.T) {
	// Classic example where naive summation fails:
	// 1e10 + 1.0 - 1e10 + 1.0 should equal 2.0
	data := []float64{1e10, 1.0, -1e10, 1.0}
	kahanSum := KahanSum(data)

	// Compute naive sum for comparison
	naiveSum := 0.0
	for _, v := range data {
		naiveSum += v
	}

	// Kahan sum should be exactly 2.0
	expected := 2.0
	if math.Abs(kahanSum-expected) > 1e-10 {
		t.Errorf("KahanSum(cancellation) = %v, want %v", kahanSum, expected)
	}

	// Log the difference to show Kahan's advantage
	t.Logf("Kahan sum: %v, Naive sum: %v, Expected: %v", kahanSum, naiveSum, expected)
}

func TestKahanSum_LargeArray(t *testing.T) {
	n := 10000
	data := make([]float64, n)

	// Fill with alternating large and small values
	for i := 0; i < n; i++ {
		if i%2 == 0 {
			data[i] = 1e8
		} else {
			data[i] = 1.0
		}
	}

	sum := KahanSum(data)
	expected := 5000.0*1e8 + 5000.0*1.0

	relativeError := math.Abs(sum-expected) / expected
	if relativeError > 1e-12 {
		t.Errorf("KahanSum(large) relative error = %v, want < 1e-12", relativeError)
	}
}

func TestKahanSum_Negative(t *testing.T) {
	data := []float64{-1.0, -2.0, -3.0, -4.0, -5.0}
	sum := KahanSum(data)
	expected := -15.0

	if math.Abs(sum-expected) > 1e-15 {
		t.Errorf("KahanSum(negative) = %v, want %v", sum, expected)
	}
}

func TestKahanSum_Mixed(t *testing.T) {
	data := []float64{10.0, -5.0, 3.0, -2.0, 1.0}
	sum := KahanSum(data)
	expected := 7.0

	if math.Abs(sum-expected) > 1e-15 {
		t.Errorf("KahanSum(mixed) = %v, want %v", sum, expected)
	}
}

func TestKahanSum_NaN(t *testing.T) {
	data := []float64{1.0, math.NaN(), 3.0}
	sum := KahanSum(data)

	if !math.IsNaN(sum) {
		t.Errorf("KahanSum(NaN) = %v, want NaN", sum)
	}
}

func TestKahanSum_Infinity(t *testing.T) {
	// Note: Kahan algorithm behavior with infinity depends on position:
	// - If infinity is the LAST element: returns Inf (no subsequent elements use NaN compensation)
	// - If infinity is NOT last: produces NaN (compensation term Inf - Inf = NaN propagates)

	// Test with infinity in the middle - produces NaN
	data := []float64{1.0, math.Inf(1), 3.0}
	sum := KahanSum(data)

	if !math.IsNaN(sum) {
		t.Errorf("KahanSum(Inf in middle) = %v, want NaN", sum)
	}

	// Test with infinity at the beginning - produces NaN
	data2 := []float64{math.Inf(1), 1.0, 2.0}
	sum2 := KahanSum(data2)

	if !math.IsNaN(sum2) {
		t.Errorf("KahanSum(Inf at start) = %v, want NaN", sum2)
	}

	// Test with infinity at the end - returns Inf
	data3 := []float64{1.0, 2.0, math.Inf(1)}
	sum3 := KahanSum(data3)

	if !math.IsInf(sum3, 1) {
		t.Errorf("KahanSum(Inf at end) = %v, want +Inf", sum3)
	}

	// Test with only infinity - returns Inf
	data4 := []float64{math.Inf(1)}
	sum4 := KahanSum(data4)

	if !math.IsInf(sum4, 1) {
		t.Errorf("KahanSum(only Inf) = %v, want +Inf", sum4)
	}
}

func TestKahanSumF32_Basic(t *testing.T) {
	data := []float32{1.0, 2.0, 3.0, 4.0, 5.0}
	sum := KahanSumF32(data)
	expected := float32(15.0)

	if math.Abs(float64(sum-expected)) > 1e-6 {
		t.Errorf("KahanSumF32() = %v, want %v", sum, expected)
	}
}

func TestKahanState_Basic(t *testing.T) {
	state := NewKahanState()

	state.Add(1.0)
	state.Add(2.0)
	state.Add(3.0)

	sum := state.Sum()
	expected := 6.0

	if math.Abs(sum-expected) > 1e-15 {
		t.Errorf("KahanState.Sum() = %v, want %v", sum, expected)
	}
}

func TestKahanState_Batch(t *testing.T) {
	state := NewKahanState()

	data1 := []float64{1.0, 2.0, 3.0}
	data2 := []float64{4.0, 5.0}

	state.AddBatch(data1)
	state.AddBatch(data2)

	sum := state.Sum()
	expected := 15.0

	if math.Abs(sum-expected) > 1e-15 {
		t.Errorf("KahanState.Sum() after batches = %v, want %v", sum, expected)
	}
}

func TestKahanState_Cancellation(t *testing.T) {
	state := NewKahanState()

	state.Add(1e10)
	state.Add(1.0)
	state.Add(-1e10)
	state.Add(1.0)

	sum := state.Sum()
	expected := 2.0

	if math.Abs(sum-expected) > 1e-10 {
		t.Errorf("KahanState.Sum(cancellation) = %v, want %v", sum, expected)
	}
}

func TestKahanState_Reset(t *testing.T) {
	state := NewKahanState()

	state.Add(1.0)
	state.Add(2.0)
	state.Add(3.0)

	state.Reset()

	sum := state.Sum()
	expected := 0.0

	if sum != expected {
		t.Errorf("KahanState.Sum() after reset = %v, want %v", sum, expected)
	}
}

func TestKahanState_EmptyBatch(t *testing.T) {
	state := NewKahanState()
	state.AddBatch([]float64{})

	sum := state.Sum()
	expected := 0.0

	if sum != expected {
		t.Errorf("KahanState.Sum() after empty batch = %v, want %v", sum, expected)
	}
}

// Benchmark tests
func BenchmarkKahanSum_Small(b *testing.B) {
	data := make([]float64, 100)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_ = KahanSum(data)
	}
}

func BenchmarkKahanSum_Medium(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_ = KahanSum(data)
	}
}

func BenchmarkKahanSum_Large(b *testing.B) {
	data := make([]float64, 1000000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_ = KahanSum(data)
	}
}

func BenchmarkNaiveSum_Large(b *testing.B) {
	data := make([]float64, 1000000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		sum := 0.0
		for _, v := range data {
			sum += v
		}
		_ = sum
	}
}

func BenchmarkKahanState_Incremental(b *testing.B) {
	data := make([]float64, 1000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		state := NewKahanState()
		for _, v := range data {
			state.Add(v)
		}
		_ = state.Sum()
	}
}

func BenchmarkKahanState_Batch(b *testing.B) {
	data := make([]float64, 1000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		state := NewKahanState()
		state.AddBatch(data)
		_ = state.Sum()
	}
}
