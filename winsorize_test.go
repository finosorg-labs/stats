package stats

import (
	"math"
	"testing"
)

const testTolerance = 1e-12

func TestWinsorizeBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}

	result, err := Winsorize(data, 0.1, 0.9)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// 10th percentile = 1.9, 90th percentile = 9.1
	if math.Abs(result[0]-1.9) > testTolerance {
		t.Errorf("Expected result[0] = 1.9, got %f", result[0])
	}
	if math.Abs(result[9]-9.1) > testTolerance {
		t.Errorf("Expected result[9] = 9.1, got %f", result[9])
	}

	// Middle values should be unchanged
	if math.Abs(result[4]-5.0) > testTolerance {
		t.Errorf("Expected result[4] = 5.0, got %f", result[4])
	}
}

func TestWinsorizeSymmetric5Pct(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0,
		11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0}

	result, err := Winsorize(data, 0.05, 0.95)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// 5th percentile = 1.95, 95th percentile = 19.05
	if math.Abs(result[0]-1.95) > testTolerance {
		t.Errorf("Expected result[0] = 1.95, got %f", result[0])
	}
	if math.Abs(result[19]-19.05) > testTolerance {
		t.Errorf("Expected result[19] = 19.05, got %f", result[19])
	}

	// Middle values should be unchanged
	if math.Abs(result[10]-11.0) > testTolerance {
		t.Errorf("Expected result[10] = 11.0, got %f", result[10])
	}
}

func TestWinsorizeWithOutliers(t *testing.T) {
	data := []float64{-100.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 100.0}

	result, err := Winsorize(data, 0.1, 0.9)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// Extreme outliers should be clamped
	if result[0] <= -100.0 {
		t.Errorf("Expected result[0] > -100.0, got %f", result[0])
	}
	if result[9] >= 100.0 {
		t.Errorf("Expected result[9] < 100.0, got %f", result[9])
	}
}

func TestWinsorizeNoChange(t *testing.T) {
	data := []float64{5.0, 5.0, 5.0, 5.0, 5.0}

	result, err := Winsorize(data, 0.1, 0.9)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// All values should remain 5.0
	for i, v := range result {
		if math.Abs(v-5.0) > testTolerance {
			t.Errorf("Expected result[%d] = 5.0, got %f", i, v)
		}
	}
}

func TestWinsorizeSingleElement(t *testing.T) {
	data := []float64{42.0}

	result, err := Winsorize(data, 0.1, 0.9)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	if math.Abs(result[0]-42.0) > testTolerance {
		t.Errorf("Expected result[0] = 42.0, got %f", result[0])
	}
}

func TestWinsorizeInPlaceBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}

	err := WinsorizeInPlace(data, 0.1, 0.9)
	if err != nil {
		t.Fatalf("WinsorizeInPlace failed: %v", err)
	}

	if math.Abs(data[0]-1.9) > testTolerance {
		t.Errorf("Expected data[0] = 1.9, got %f", data[0])
	}
	if math.Abs(data[9]-9.1) > testTolerance {
		t.Errorf("Expected data[9] = 9.1, got %f", data[9])
	}
}

func TestWinsorizeInPlacePreservesMiddle(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}

	err := WinsorizeInPlace(data, 0.2, 0.8)
	if err != nil {
		t.Fatalf("WinsorizeInPlace failed: %v", err)
	}

	// Middle values should be unchanged
	if math.Abs(data[4]-5.0) > testTolerance {
		t.Errorf("Expected data[4] = 5.0, got %f", data[4])
	}
	if math.Abs(data[5]-6.0) > testTolerance {
		t.Errorf("Expected data[5] = 6.0, got %f", data[5])
	}
}

func TestWinsorizeBatchBasic(t *testing.T) {
	data1 := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	data2 := []float64{10.0, 20.0, 30.0, 40.0, 50.0}
	data3 := []float64{-5.0, -3.0, 0.0, 3.0, 5.0}

	data := [][]float64{data1, data2, data3}

	results, err := WinsorizeBatch(data, 0.2, 0.8)
	if err != nil {
		t.Fatalf("WinsorizeBatch failed: %v", err)
	}

	if len(results) != 3 {
		t.Fatalf("Expected 3 results, got %d", len(results))
	}

	// Check first array
	if math.Abs(results[0][0]-1.8) > testTolerance {
		t.Errorf("Expected results[0][0] = 1.8, got %f", results[0][0])
	}
	if math.Abs(results[0][4]-4.2) > testTolerance {
		t.Errorf("Expected results[0][4] = 4.2, got %f", results[0][4])
	}

	// Check second array
	if math.Abs(results[1][0]-18.0) > testTolerance {
		t.Errorf("Expected results[1][0] = 18.0, got %f", results[1][0])
	}
	if math.Abs(results[1][4]-42.0) > testTolerance {
		t.Errorf("Expected results[1][4] = 42.0, got %f", results[1][4])
	}
}

func TestWinsorizeBatchIndependent(t *testing.T) {
	data1 := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	data2 := []float64{100.0, 200.0, 300.0, 400.0, 500.0}

	data := [][]float64{data1, data2}

	results, err := WinsorizeBatch(data, 0.1, 0.9)
	if err != nil {
		t.Fatalf("WinsorizeBatch failed: %v", err)
	}

	// Each array should be winsorized independently
	// data1: 10th pct = 1.4, 90th pct = 4.6
	if math.Abs(results[0][0]-1.4) > testTolerance {
		t.Errorf("Expected results[0][0] = 1.4, got %f", results[0][0])
	}
	if math.Abs(results[0][4]-4.6) > testTolerance {
		t.Errorf("Expected results[0][4] = 4.6, got %f", results[0][4])
	}

	// data2: 10th pct = 140, 90th pct = 460
	if math.Abs(results[1][0]-140.0) > testTolerance {
		t.Errorf("Expected results[1][0] = 140.0, got %f", results[1][0])
	}
	if math.Abs(results[1][4]-460.0) > testTolerance {
		t.Errorf("Expected results[1][4] = 460.0, got %f", results[1][4])
	}
}

func TestWinsorizeEmptySlice(t *testing.T) {
	data := []float64{}

	_, err := Winsorize(data, 0.1, 0.9)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg, got %v", err)
	}
}

func TestWinsorizeInvalidPercentiles(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}

	// lower >= upper
	_, err := Winsorize(data, 0.9, 0.1)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg for lower >= upper, got %v", err)
	}

	// lower == upper
	_, err = Winsorize(data, 0.5, 0.5)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg for lower == upper, got %v", err)
	}

	// out of range
	_, err = Winsorize(data, -0.1, 0.9)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg for negative percentile, got %v", err)
	}

	_, err = Winsorize(data, 0.1, 1.1)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg for percentile > 1, got %v", err)
	}
}

func TestWinsorizeNaNInput(t *testing.T) {
	data := []float64{1.0, 2.0, math.NaN(), 4.0, 5.0}

	_, err := Winsorize(data, 0.1, 0.9)
	if err != ErrNaNInput {
		t.Errorf("Expected ErrNaNInput, got %v", err)
	}
}

func TestWinsorizeBatchEmptySlice(t *testing.T) {
	data := [][]float64{}

	_, err := WinsorizeBatch(data, 0.1, 0.9)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg, got %v", err)
	}
}

func TestWinsorizeBatchUnequalLengths(t *testing.T) {
	data1 := []float64{1.0, 2.0, 3.0}
	data2 := []float64{4.0, 5.0}

	data := [][]float64{data1, data2}

	_, err := WinsorizeBatch(data, 0.1, 0.9)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg for unequal lengths, got %v", err)
	}
}

func TestWinsorizeExtremePercentiles(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}

	// Very narrow range
	result, err := Winsorize(data, 0.4, 0.6)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// All values should be clamped to middle range
	for i, v := range result {
		if v < 2.6 || v > 3.4 {
			t.Errorf("Expected result[%d] in [2.6, 3.4], got %f", i, v)
		}
	}
}

func TestWinsorizeWidePercentiles(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}

	// Very wide range - should change almost nothing
	result, err := Winsorize(data, 0.01, 0.99)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// Most values should be unchanged
	for i := 1; i < 4; i++ {
		if math.Abs(result[i]-data[i]) > testTolerance {
			t.Errorf("Expected result[%d] = %f, got %f", i, data[i], result[i])
		}
	}
}

func TestWinsorizeNegativeValues(t *testing.T) {
	data := []float64{-10.0, -5.0, -3.0, -1.0, 0.0, 1.0, 3.0, 5.0, 10.0}

	result, err := Winsorize(data, 0.1, 0.9)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// Check that negative values are handled correctly
	if result[0] <= -10.0 {
		t.Errorf("Expected result[0] > -10.0, got %f", result[0])
	}
	if result[8] >= 10.0 {
		t.Errorf("Expected result[8] < 10.0, got %f", result[8])
	}
}

func TestWinsorizeLargeArray(t *testing.T) {
	n := 10000
	data := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(i)
	}

	result, err := Winsorize(data, 0.05, 0.95)
	if err != nil {
		t.Fatalf("Winsorize failed: %v", err)
	}

	// Check that extreme values are clamped
	if result[0] <= 0.0 {
		t.Errorf("Expected result[0] > 0.0, got %f", result[0])
	}
	if result[n-1] >= float64(n-1) {
		t.Errorf("Expected result[%d] < %f, got %f", n-1, float64(n-1), result[n-1])
	}

	// Check that middle values are unchanged
	mid := n / 2
	if math.Abs(result[mid]-float64(mid)) > testTolerance {
		t.Errorf("Expected result[%d] = %f, got %f", mid, float64(mid), result[mid])
	}
}

// Benchmark tests

func BenchmarkWinsorize1K(b *testing.B) {
	data := make([]float64, 1000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Winsorize(data, 0.05, 0.95)
	}
}

func BenchmarkWinsorize10K(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Winsorize(data, 0.05, 0.95)
	}
}

func BenchmarkWinsorize100K(b *testing.B) {
	data := make([]float64, 100000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Winsorize(data, 0.05, 0.95)
	}
}

func BenchmarkWinsorizeInPlace100K(b *testing.B) {
	original := make([]float64, 100000)
	for i := range original {
		original[i] = float64(i)
	}

	data := make([]float64, 100000)

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		copy(data, original)
		_ = WinsorizeInPlace(data, 0.05, 0.95)
	}
}

func BenchmarkWinsorizeBatch10x10K(b *testing.B) {
	data := make([][]float64, 10)
	for i := range data {
		data[i] = make([]float64, 10000)
		for j := range data[i] {
			data[i][j] = float64(j)
		}
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WinsorizeBatch(data, 0.05, 0.95)
	}
}
