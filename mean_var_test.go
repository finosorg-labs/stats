package stats

import (
	"math"
	"testing"
)

const tolerance = 1e-12

func almostEqual(a, b, tol float64) bool {
	diff := math.Abs(a - b)
	if b != 0 {
		return diff/math.Abs(b) < tol
	}
	return diff < tol
}

// TestMeanBasic tests basic mean computation
func TestMeanBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	mean, err := Mean(data)

	if err != nil {
		t.Fatalf("Mean failed: %v", err)
	}

	expected := 3.0
	if !almostEqual(mean, expected, tolerance) {
		t.Errorf("Mean = %v, expected %v", mean, expected)
	}
}

// TestMeanSingleElement tests mean with single element
func TestMeanSingleElement(t *testing.T) {
	data := []float64{42.0}
	mean, err := Mean(data)

	if err != nil {
		t.Fatalf("Mean failed: %v", err)
	}

	if !almostEqual(mean, 42.0, tolerance) {
		t.Errorf("Mean = %v, expected 42.0", mean)
	}
}

// TestMeanEmpty tests mean with empty slice
func TestMeanEmpty(t *testing.T) {
	data := []float64{}
	_, err := Mean(data)

	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg, got %v", err)
	}
}

// TestVarianceSample tests sample variance
func TestVarianceSample(t *testing.T) {
	data := []float64{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0}
	variance, err := Variance(data, true)

	if err != nil {
		t.Fatalf("Variance failed: %v", err)
	}

	expected := 4.571428571428571 // sample variance = m2 / (n-1)
	if !almostEqual(variance, expected, tolerance) {
		t.Errorf("Variance = %v, expected %v", variance, expected)
	}
}

// TestVariancePopulation tests population variance
func TestVariancePopulation(t *testing.T) {
	data := []float64{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0}
	variance, err := Variance(data, false)

	if err != nil {
		t.Fatalf("Variance failed: %v", err)
	}

	expected := 4.0 // population variance = m2 / n
	if !almostEqual(variance, expected, tolerance) {
		t.Errorf("Variance = %v, expected %v", variance, expected)
	}
}

// TestVarianceConstant tests variance of constant values
func TestVarianceConstant(t *testing.T) {
	data := []float64{5.0, 5.0, 5.0, 5.0, 5.0}
	variance, err := Variance(data, true)

	if err != nil {
		t.Fatalf("Variance failed: %v", err)
	}

	if !almostEqual(variance, 0.0, tolerance) {
		t.Errorf("Variance = %v, expected 0.0", variance)
	}
}

// TestMeanVariance tests combined mean and variance
func TestMeanVariance(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	mean, variance, err := MeanVariance(data, true)

	if err != nil {
		t.Fatalf("MeanVariance failed: %v", err)
	}

	if !almostEqual(mean, 3.0, tolerance) {
		t.Errorf("Mean = %v, expected 3.0", mean)
	}

	if !almostEqual(variance, 2.5, tolerance) {
		t.Errorf("Variance = %v, expected 2.5", variance)
	}
}

// TestStdDev tests standard deviation
func TestStdDev(t *testing.T) {
	data := []float64{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0}
	stddev, err := StdDev(data, true)

	if err != nil {
		t.Fatalf("StdDev failed: %v", err)
	}

	expected := 2.138089935299395 // sqrt(sample variance)
	if !almostEqual(stddev, expected, tolerance) {
		t.Errorf("StdDev = %v, expected %v", stddev, expected)
	}
}

// TestMeanBatch tests batch mean computation
func TestMeanBatch(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0},
		{4.0, 5.0, 6.0},
		{7.0, 8.0, 9.0},
	}

	means, err := MeanBatch(data)
	if err != nil {
		t.Fatalf("MeanBatch failed: %v", err)
	}

	expected := []float64{2.0, 5.0, 8.0}
	for i, mean := range means {
		if !almostEqual(mean, expected[i], tolerance) {
			t.Errorf("Mean[%d] = %v, expected %v", i, mean, expected[i])
		}
	}
}

// TestVarianceBatch tests batch variance computation
func TestVarianceBatch(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0, 4.0, 5.0},
		{2.0, 4.0, 6.0, 8.0, 10.0},
	}

	variances, err := VarianceBatch(data, true)
	if err != nil {
		t.Fatalf("VarianceBatch failed: %v", err)
	}

	expected := []float64{2.5, 10.0}
	for i, variance := range variances {
		if !almostEqual(variance, expected[i], tolerance) {
			t.Errorf("Variance[%d] = %v, expected %v", i, variance, expected[i])
		}
	}
}

// TestMeanVarianceBatch tests batch mean and variance computation
func TestMeanVarianceBatch(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0, 4.0, 5.0},
		{10.0, 20.0, 30.0, 40.0, 50.0},
	}

	means, variances, err := MeanVarianceBatch(data, true)
	if err != nil {
		t.Fatalf("MeanVarianceBatch failed: %v", err)
	}

	expectedMeans := []float64{3.0, 30.0}
	expectedVariances := []float64{2.5, 250.0}

	for i := range means {
		if !almostEqual(means[i], expectedMeans[i], tolerance) {
			t.Errorf("Mean[%d] = %v, expected %v", i, means[i], expectedMeans[i])
		}
		if !almostEqual(variances[i], expectedVariances[i], tolerance) {
			t.Errorf("Variance[%d] = %v, expected %v", i, variances[i], expectedVariances[i])
		}
	}
}

// TestMeanLargeDataset tests mean with large dataset
func TestMeanLargeDataset(t *testing.T) {
	n := 10000
	data := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(i)
	}

	mean, err := Mean(data)
	if err != nil {
		t.Fatalf("Mean failed: %v", err)
	}

	expected := 4999.5
	if !almostEqual(mean, expected, tolerance) {
		t.Errorf("Mean = %v, expected %v", mean, expected)
	}
}

// TestVarianceLargeDataset tests variance with large dataset
func TestVarianceLargeDataset(t *testing.T) {
	n := 1000
	data := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(i)
	}

	variance, err := Variance(data, false)
	if err != nil {
		t.Fatalf("Variance failed: %v", err)
	}

	// Population variance: (n^2 - 1) / 12
	expected := (float64(n)*float64(n) - 1.0) / 12.0
	if !almostEqual(variance, expected, 1e-8) {
		t.Errorf("Variance = %v, expected %v", variance, expected)
	}
}

// TestNaNInput tests NaN handling
func TestNaNInput(t *testing.T) {
	data := []float64{1.0, 2.0, math.NaN(), 4.0, 5.0}
	_, err := Mean(data)

	if err != ErrNaNInput {
		t.Errorf("Expected ErrNaNInput, got %v", err)
	}
}

// TestInsufficientSamples tests error for insufficient samples
func TestInsufficientSamples(t *testing.T) {
	data := []float64{1.0}
	_, err := Variance(data, true)

	if err != ErrInsufficientData {
		t.Errorf("Expected ErrInsufficientData, got %v", err)
	}
}

// BenchmarkMean benchmarks mean computation
func BenchmarkMean(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Mean(data)
	}
}

// BenchmarkVariance benchmarks variance computation
func BenchmarkVariance(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Variance(data, true)
	}
}

// BenchmarkMeanVariance benchmarks combined mean and variance
func BenchmarkMeanVariance(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _, _ = MeanVariance(data, true)
	}
}

// BenchmarkMeanVarianceBatch benchmarks batch operation
func BenchmarkMeanVarianceBatch(b *testing.B) {
	numArrays := 5000
	n := 250
	data := make([][]float64, numArrays)
	for i := range data {
		data[i] = make([]float64, n)
		for j := range data[i] {
			data[i][j] = float64(j)
		}
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _, _ = MeanVarianceBatch(data, true)
	}
}

