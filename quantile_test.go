package stats

import (
	"math"
	"testing"
)

func TestQuantileMedianOdd(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	result, err := Quantile(data, 0.5)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-3.0) > tolerance {
		t.Errorf("Expected 3.0, got %f", result)
	}
}

func TestQuantileMedianEven(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}
	result, err := Quantile(data, 0.5)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-3.5) > tolerance {
		t.Errorf("Expected 3.5, got %f", result)
	}
}

func TestQuantileQ1(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0}
	result, err := Quantile(data, 0.25)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	expected := 2.75
	if math.Abs(result-expected) > tolerance {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestQuantileQ3(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0}
	result, err := Quantile(data, 0.75)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	expected := 6.25
	if math.Abs(result-expected) > tolerance {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestQuantileMin(t *testing.T) {
	data := []float64{5.0, 2.0, 8.0, 1.0, 9.0}
	result, err := Quantile(data, 0.0)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-1.0) > tolerance {
		t.Errorf("Expected 1.0, got %f", result)
	}
}

func TestQuantileMax(t *testing.T) {
	data := []float64{5.0, 2.0, 8.0, 1.0, 9.0}
	result, err := Quantile(data, 1.0)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-9.0) > tolerance {
		t.Errorf("Expected 9.0, got %f", result)
	}
}

func TestQuantileSingleElement(t *testing.T) {
	data := []float64{42.0}
	result, err := Quantile(data, 0.5)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-42.0) > tolerance {
		t.Errorf("Expected 42.0, got %f", result)
	}
}

func TestQuantileAllSame(t *testing.T) {
	data := []float64{5.0, 5.0, 5.0, 5.0, 5.0}
	result, err := Quantile(data, 0.5)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-5.0) > tolerance {
		t.Errorf("Expected 5.0, got %f", result)
	}
}

func TestQuantileUnsorted(t *testing.T) {
	data := []float64{9.0, 1.0, 5.0, 3.0, 7.0, 2.0, 8.0, 4.0, 6.0}
	result, err := Quantile(data, 0.5)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-5.0) > tolerance {
		t.Errorf("Expected 5.0, got %f", result)
	}
}

func TestQuantileEmptyData(t *testing.T) {
	data := []float64{}
	_, err := Quantile(data, 0.5)
	if err == nil {
		t.Error("Expected error for empty data")
	}
}

func TestQuantileInvalidQ(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}

	_, err := Quantile(data, -0.1)
	if err == nil {
		t.Error("Expected error for q < 0")
	}

	_, err = Quantile(data, 1.1)
	if err == nil {
		t.Error("Expected error for q > 1")
	}
}

func TestQuantileNaN(t *testing.T) {
	data := []float64{1.0, 2.0, math.NaN(), 4.0, 5.0}
	_, err := Quantile(data, 0.5)
	if err == nil {
		t.Error("Expected error for NaN in data")
	}
}

func TestQuantilesMultiple(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}
	quantiles := []float64{0.25, 0.5, 0.75}
	results, err := Quantiles(data, quantiles)
	if err != nil {
		t.Fatalf("Quantiles failed: %v", err)
	}

	expected := []float64{3.25, 5.5, 7.75}
	for i, exp := range expected {
		if math.Abs(results[i]-exp) > tolerance {
			t.Errorf("Quantile[%d]: expected %f, got %f", i, exp, results[i])
		}
	}
}

func TestQuantilesEmptyQuantiles(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	quantiles := []float64{}
	_, err := Quantiles(data, quantiles)
	if err == nil {
		t.Error("Expected error for empty quantiles")
	}
}

func TestMedian(t *testing.T) {
	data := []float64{5.0, 2.0, 8.0, 1.0, 9.0, 3.0, 7.0}
	result, err := Median(data)
	if err != nil {
		t.Fatalf("Median failed: %v", err)
	}
	if math.Abs(result-5.0) > tolerance {
		t.Errorf("Expected 5.0, got %f", result)
	}
}

func TestMedianEvenLength(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0}
	result, err := Median(data)
	if err != nil {
		t.Fatalf("Median failed: %v", err)
	}
	if math.Abs(result-2.5) > tolerance {
		t.Errorf("Expected 2.5, got %f", result)
	}
}

func TestQuartiles(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}
	q1, q2, q3, err := Quartiles(data)
	if err != nil {
		t.Fatalf("Quartiles failed: %v", err)
	}

	if math.Abs(q1-3.25) > tolerance {
		t.Errorf("Q1: expected 3.25, got %f", q1)
	}
	if math.Abs(q2-5.5) > tolerance {
		t.Errorf("Q2: expected 5.5, got %f", q2)
	}
	if math.Abs(q3-7.75) > tolerance {
		t.Errorf("Q3: expected 7.75, got %f", q3)
	}
}

func TestQuantileNonDestructive(t *testing.T) {
	original := []float64{5.0, 2.0, 8.0, 1.0, 9.0}
	data := make([]float64, len(original))
	copy(data, original)

	result, err := QuantileNonDestructive(data, 0.5)
	if err != nil {
		t.Fatalf("QuantileNonDestructive failed: %v", err)
	}

	// Verify original data is unchanged
	for i := range data {
		if data[i] != original[i] {
			t.Errorf("Data was modified at index %d: expected %f, got %f", i, original[i], data[i])
		}
	}

	if math.Abs(result-5.0) > tolerance {
		t.Errorf("Expected 5.0, got %f", result)
	}
}

func TestMedianNonDestructive(t *testing.T) {
	original := []float64{5.0, 2.0, 8.0, 1.0, 9.0}
	data := make([]float64, len(original))
	copy(data, original)

	result, err := MedianNonDestructive(data)
	if err != nil {
		t.Fatalf("MedianNonDestructive failed: %v", err)
	}

	// Verify original data is unchanged
	for i := range data {
		if data[i] != original[i] {
			t.Errorf("Data was modified at index %d: expected %f, got %f", i, original[i], data[i])
		}
	}

	if math.Abs(result-5.0) > tolerance {
		t.Errorf("Expected 5.0, got %f", result)
	}
}

func TestQuartilesNonDestructive(t *testing.T) {
	original := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}
	data := make([]float64, len(original))
	copy(data, original)

	q1, q2, q3, err := QuartilesNonDestructive(data)
	if err != nil {
		t.Fatalf("QuartilesNonDestructive failed: %v", err)
	}

	// Verify original data is unchanged
	for i := range data {
		if data[i] != original[i] {
			t.Errorf("Data was modified at index %d: expected %f, got %f", i, original[i], data[i])
		}
	}

	if math.Abs(q1-3.25) > tolerance {
		t.Errorf("Q1: expected 3.25, got %f", q1)
	}
	if math.Abs(q2-5.5) > tolerance {
		t.Errorf("Q2: expected 5.5, got %f", q2)
	}
	if math.Abs(q3-7.75) > tolerance {
		t.Errorf("Q3: expected 7.75, got %f", q3)
	}
}

func TestQuantileLargeDataset(t *testing.T) {
	n := 10000
	data := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(i)
	}

	result, err := Quantile(data, 0.5)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}

	expected := 4999.5
	if math.Abs(result-expected) > tolerance {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestQuantileNegativeValues(t *testing.T) {
	data := []float64{-5.0, -3.0, -1.0, 1.0, 3.0, 5.0}
	result, err := Quantile(data, 0.5)
	if err != nil {
		t.Fatalf("Quantile failed: %v", err)
	}
	if math.Abs(result-0.0) > tolerance {
		t.Errorf("Expected 0.0, got %f", result)
	}
}

// Benchmark tests

func BenchmarkMedianSmall(b *testing.B) {
	data := make([]float64, 100)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		temp := make([]float64, len(data))
		copy(temp, data)
		_, _ = Median(temp)
	}
}

func BenchmarkMedianMedium(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		temp := make([]float64, len(data))
		copy(temp, data)
		_, _ = Median(temp)
	}
}

func BenchmarkMedianLarge(b *testing.B) {
	data := make([]float64, 1000000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		temp := make([]float64, len(data))
		copy(temp, data)
		_, _ = Median(temp)
	}
}

func BenchmarkQuartiles(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		temp := make([]float64, len(data))
		copy(temp, data)
		_, _, _, _ = Quartiles(temp)
	}
}

func BenchmarkQuantileMultiple(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}
	quantiles := []float64{0.25, 0.5, 0.75}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		temp := make([]float64, len(data))
		copy(temp, data)
		_, _ = Quantiles(temp, quantiles)
	}
}
