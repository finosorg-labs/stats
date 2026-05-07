package stats

import (
	"math"
	"testing"
)

func TestRollingMeanBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	windowSize := 3

	result, err := RollingMean(data, windowSize)
	if err != nil {
		t.Fatalf("RollingMean failed: %v", err)
	}

	expected := []float64{1.0, 1.5, 2.0, 3.0, 4.0}
	if len(result) != len(expected) {
		t.Fatalf("Expected length %d, got %d", len(expected), len(result))
	}

	for i := range result {
		if math.Abs(result[i]-expected[i]) > epsilon {
			t.Errorf("result[%d] = %f, expected %f", i, result[i], expected[i])
		}
	}
}

func TestRollingMeanWindowOne(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	windowSize := 1

	result, err := RollingMean(data, windowSize)
	if err != nil {
		t.Fatalf("RollingMean failed: %v", err)
	}

	for i := range result {
		if math.Abs(result[i]-data[i]) > epsilon {
			t.Errorf("result[%d] = %f, expected %f", i, result[i], data[i])
		}
	}
}

func TestRollingVarianceBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	windowSize := 3

	result, err := RollingVariance(data, windowSize, true)
	if err != nil {
		t.Fatalf("RollingVariance failed: %v", err)
	}

	expected := []float64{0.0, 0.5, 1.0, 1.0, 1.0}
	if len(result) != len(expected) {
		t.Fatalf("Expected length %d, got %d", len(expected), len(result))
	}

	for i := range result {
		if math.Abs(result[i]-expected[i]) > epsilon {
			t.Errorf("result[%d] = %f, expected %f", i, result[i], expected[i])
		}
	}
}

func TestRollingVariancePopulation(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	windowSize := 3

	result, err := RollingVariance(data, windowSize, false)
	if err != nil {
		t.Fatalf("RollingVariance failed: %v", err)
	}

	expected := 2.0 / 3.0
	for i := 2; i < len(result); i++ {
		if math.Abs(result[i]-expected) > epsilon {
			t.Errorf("result[%d] = %f, expected %f", i, result[i], expected)
		}
	}
}

func TestRollingStddevBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	windowSize := 3

	result, err := RollingStddev(data, windowSize, true)
	if err != nil {
		t.Fatalf("RollingStddev failed: %v", err)
	}

	for i := 2; i < len(result); i++ {
		if math.Abs(result[i]-1.0) > epsilon {
			t.Errorf("result[%d] = %f, expected 1.0", i, result[i])
		}
	}
}

func TestRollingSumBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	windowSize := 3

	result, err := RollingSum(data, windowSize)
	if err != nil {
		t.Fatalf("RollingSum failed: %v", err)
	}

	expected := []float64{1.0, 3.0, 6.0, 9.0, 12.0}
	if len(result) != len(expected) {
		t.Fatalf("Expected length %d, got %d", len(expected), len(result))
	}

	for i := range result {
		if math.Abs(result[i]-expected[i]) > epsilon {
			t.Errorf("result[%d] = %f, expected %f", i, result[i], expected[i])
		}
	}
}

func TestRollingMinBasic(t *testing.T) {
	data := []float64{5.0, 2.0, 8.0, 1.0, 9.0}
	windowSize := 3

	result, err := RollingMin(data, windowSize)
	if err != nil {
		t.Fatalf("RollingMin failed: %v", err)
	}

	expected := []float64{5.0, 2.0, 2.0, 1.0, 1.0}
	if len(result) != len(expected) {
		t.Fatalf("Expected length %d, got %d", len(expected), len(result))
	}

	for i := range result {
		if math.Abs(result[i]-expected[i]) > epsilon {
			t.Errorf("result[%d] = %f, expected %f", i, result[i], expected[i])
		}
	}
}

func TestRollingMaxBasic(t *testing.T) {
	data := []float64{5.0, 2.0, 8.0, 1.0, 9.0}
	windowSize := 3

	result, err := RollingMax(data, windowSize)
	if err != nil {
		t.Fatalf("RollingMax failed: %v", err)
	}

	expected := []float64{5.0, 5.0, 8.0, 8.0, 9.0}
	if len(result) != len(expected) {
		t.Fatalf("Expected length %d, got %d", len(expected), len(result))
	}

	for i := range result {
		if math.Abs(result[i]-expected[i]) > epsilon {
			t.Errorf("result[%d] = %f, expected %f", i, result[i], expected[i])
		}
	}
}

func TestRollingMeanLargeWindow(t *testing.T) {
	n := 1000
	windowSize := 250
	data := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(i + 1)
	}

	result, err := RollingMean(data, windowSize)
	if err != nil {
		t.Fatalf("RollingMean failed: %v", err)
	}

	if len(result) != n {
		t.Fatalf("Expected length %d, got %d", n, len(result))
	}

	expected := 0.0
	for i := 0; i < windowSize; i++ {
		expected += data[i]
	}
	expected /= float64(windowSize)

	if math.Abs(result[windowSize-1]-expected) > epsilon {
		t.Errorf("result[%d] = %f, expected %f", windowSize-1, result[windowSize-1], expected)
	}
}

func TestRollingMeanEmptyData(t *testing.T) {
	data := []float64{}
	_, err := RollingMean(data, 3)
	if err == nil {
		t.Error("Expected error for empty data")
	}
}

func TestRollingMeanInvalidWindow(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}

	_, err := RollingMean(data, 0)
	if err == nil {
		t.Error("Expected error for window size 0")
	}

	_, err = RollingMean(data, 4)
	if err == nil {
		t.Error("Expected error for window size > data length")
	}
}

func TestRollingMeanConstant(t *testing.T) {
	data := make([]float64, 10)
	for i := range data {
		data[i] = 42.0
	}

	result, err := RollingMean(data, 5)
	if err != nil {
		t.Fatalf("RollingMean failed: %v", err)
	}

	for i := range result {
		if math.Abs(result[i]-42.0) > epsilon {
			t.Errorf("result[%d] = %f, expected 42.0", i, result[i])
		}
	}
}

func TestRollingVarianceConstant(t *testing.T) {
	data := make([]float64, 10)
	for i := range data {
		data[i] = 42.0
	}

	result, err := RollingVariance(data, 5, true)
	if err != nil {
		t.Fatalf("RollingVariance failed: %v", err)
	}

	for i := 1; i < len(result); i++ {
		if math.Abs(result[i]) > epsilon {
			t.Errorf("result[%d] = %f, expected 0.0", i, result[i])
		}
	}
}

// Benchmarks

func BenchmarkRollingMeanSmall(b *testing.B) {
	data := make([]float64, 1000)
	for i := range data {
		data[i] = float64(i)
	}
	windowSize := 50

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = RollingMean(data, windowSize)
	}
}

func BenchmarkRollingMeanMedium(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}
	windowSize := 250

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = RollingMean(data, windowSize)
	}
}

func BenchmarkRollingMeanLarge(b *testing.B) {
	data := make([]float64, 1000000)
	for i := range data {
		data[i] = float64(i)
	}
	windowSize := 250

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = RollingMean(data, windowSize)
	}
}

func BenchmarkRollingVarianceMedium(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}
	windowSize := 250

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = RollingVariance(data, windowSize, true)
	}
}

func BenchmarkRollingSumLarge(b *testing.B) {
	data := make([]float64, 1000000)
	for i := range data {
		data[i] = float64(i)
	}
	windowSize := 250

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = RollingSum(data, windowSize)
	}
}
