package stats

import (
	"math"
	"testing"
)

func TestWeightedMean(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 2.0}

	mean, err := WeightedMean(data, weights)
	if err != nil {
		t.Fatalf("WeightedMean failed: %v", err)
	}
	if !almostEqual(mean, 2.25, tolerance) {
		t.Errorf("WeightedMean = %v, expected 2.25", mean)
	}
}

func TestWeightedVariance(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 2.0}

	variance, err := WeightedVariance(data, weights)
	if err != nil {
		t.Fatalf("WeightedVariance failed: %v", err)
	}
	if !almostEqual(variance, 0.6875, tolerance) {
		t.Errorf("WeightedVariance = %v, expected 0.6875", variance)
	}
}

func TestWeightedMeanVariance(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 2.0}

	mean, variance, err := WeightedMeanVariance(data, weights)
	if err != nil {
		t.Fatalf("WeightedMeanVariance failed: %v", err)
	}
	if !almostEqual(mean, 2.25, tolerance) {
		t.Errorf("mean = %v, expected 2.25", mean)
	}
	if !almostEqual(variance, 0.6875, tolerance) {
		t.Errorf("variance = %v, expected 0.6875", variance)
	}
}

func TestWeightedStdDev(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 2.0}

	stddev, err := WeightedStdDev(data, weights)
	if err != nil {
		t.Fatalf("WeightedStdDev failed: %v", err)
	}
	if !almostEqual(stddev, math.Sqrt(0.6875), tolerance) {
		t.Errorf("WeightedStdDev = %v, expected %v", stddev, math.Sqrt(0.6875))
	}
}

func TestWeightedZeroWeightsAllowed(t *testing.T) {
	data := []float64{10.0, 20.0, 30.0}
	weights := []float64{0.0, 1.0, 0.0}

	mean, variance, err := WeightedMeanVariance(data, weights)
	if err != nil {
		t.Fatalf("WeightedMeanVariance failed: %v", err)
	}
	if !almostEqual(mean, 20.0, tolerance) {
		t.Errorf("mean = %v, expected 20", mean)
	}
	if !almostEqual(variance, 0.0, tolerance) {
		t.Errorf("variance = %v, expected 0", variance)
	}
}

func TestWeightedInvalidInputs(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 1.0}

	if _, err := WeightedMean(nil, nil); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for empty input, got %v", err)
	}
	if _, err := WeightedMean(data, weights[:2]); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for length mismatch, got %v", err)
	}
	if _, err := WeightedMean(data, []float64{0.0, 0.0, 0.0}); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for zero total weight, got %v", err)
	}
	if _, err := WeightedMean(data, []float64{1.0, -1.0, 1.0}); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for negative weight, got %v", err)
	}
	if _, err := WeightedMean([]float64{1.0, math.NaN(), 3.0}, weights); err != ErrNaNInput {
		t.Errorf("expected ErrNaNInput for NaN data, got %v", err)
	}
	if _, err := WeightedMean(data, []float64{1.0, math.NaN(), 1.0}); err != ErrNaNInput {
		t.Errorf("expected ErrNaNInput for NaN weight, got %v", err)
	}
}

func TestWeightedBatch(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 10.0, 20.0, 30.0}
	weights := []float64{1.0, 1.0, 2.0, 1.0, 2.0, 1.0}

	means, err := WeightedMeanBatch(data, weights, 2, 3)
	if err != nil {
		t.Fatalf("WeightedMeanBatch failed: %v", err)
	}
	if !almostEqual(means[0], 2.25, tolerance) || !almostEqual(means[1], 20.0, tolerance) {
		t.Errorf("means = %v, expected [2.25 20]", means)
	}

	variances, err := WeightedVarianceBatch(data, weights, 2, 3)
	if err != nil {
		t.Fatalf("WeightedVarianceBatch failed: %v", err)
	}
	if !almostEqual(variances[0], 0.6875, tolerance) || !almostEqual(variances[1], 50.0, tolerance) {
		t.Errorf("variances = %v, expected [0.6875 50]", variances)
	}

	combinedMeans, combinedVariances, err := WeightedMeanVarianceBatch(data, weights, 2, 3)
	if err != nil {
		t.Fatalf("WeightedMeanVarianceBatch failed: %v", err)
	}
	if !almostEqual(combinedMeans[0], 2.25, tolerance) || !almostEqual(combinedMeans[1], 20.0, tolerance) {
		t.Errorf("combined means = %v, expected [2.25 20]", combinedMeans)
	}
	if !almostEqual(combinedVariances[0], 0.6875, tolerance) || !almostEqual(combinedVariances[1], 50.0, tolerance) {
		t.Errorf("combined variances = %v, expected [0.6875 50]", combinedVariances)
	}

	stddevs, err := WeightedStdDevBatch(data, weights, 2, 3)
	if err != nil {
		t.Fatalf("WeightedStdDevBatch failed: %v", err)
	}
	if !almostEqual(stddevs[0], math.Sqrt(0.6875), tolerance) || !almostEqual(stddevs[1], math.Sqrt(50.0), tolerance) {
		t.Errorf("stddevs = %v, expected [%v %v]", stddevs, math.Sqrt(0.6875), math.Sqrt(50.0))
	}
}

func TestWeightedBatchInvalidInputs(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 1.0}

	if _, err := WeightedMeanBatch(data, weights, 0, 3); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for zero groups, got %v", err)
	}
	if _, err := WeightedMeanBatch(data, weights, 1, 0); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for zero group size, got %v", err)
	}
	if _, err := WeightedMeanBatch(data, weights, 2, 3); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for dimension mismatch, got %v", err)
	}
	if _, err := WeightedMeanBatch(data, []float64{0.0, 0.0, 0.0}, 1, 3); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for zero total weight, got %v", err)
	}
	if _, err := WeightedMeanBatch([]float64{1.0, math.NaN(), 3.0}, weights, 1, 3); err != ErrNaNInput {
		t.Errorf("expected ErrNaNInput for NaN batch, got %v", err)
	}
}

func BenchmarkWeightedMean(b *testing.B) {
	data, weights := weightedBenchmarkData(10000)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMean(data, weights)
	}
}

func BenchmarkWeightedVariance(b *testing.B) {
	data, weights := weightedBenchmarkData(10000)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedVariance(data, weights)
	}
}

func BenchmarkWeightedMeanVariance(b *testing.B) {
	data, weights := weightedBenchmarkData(10000)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _, _ = WeightedMeanVariance(data, weights)
	}
}

func BenchmarkWeightedMeanVarianceBatch(b *testing.B) {
	data, weights := weightedBenchmarkData(100000)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _, _ = WeightedMeanVarianceBatch(data, weights, 100, 1000)
	}
}

func weightedBenchmarkData(n int) ([]float64, []float64) {
	data := make([]float64, n)
	weights := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(i%1000) - 500.0
		weights[i] = float64(i%10) + 1.0
	}
	return data, weights
}
