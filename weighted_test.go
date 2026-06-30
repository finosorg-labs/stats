package stats

import (
	"math"
	"testing"
)

func TestWeightedSum(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 2.0}

	sum, err := WeightedSum(data, weights)
	if err != nil {
		t.Fatalf("WeightedSum failed: %v", err)
	}
	if !almostEqual(sum, 9.0, tolerance) {
		t.Errorf("WeightedSum = %v, expected 9.0", sum)
	}
}

func TestWeightedSumZeroWeights(t *testing.T) {
	data := []float64{10.0, 20.0, 30.0}
	weights := []float64{0.0, 1.0, 0.0}

	sum, err := WeightedSum(data, weights)
	if err != nil {
		t.Fatalf("WeightedSum failed: %v", err)
	}
	if !almostEqual(sum, 20.0, tolerance) {
		t.Errorf("WeightedSum = %v, expected 20.0", sum)
	}
}

func TestWeightedSumAllZeroWeights(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{0.0, 0.0, 0.0}

	sum, err := WeightedSum(data, weights)
	if err != nil {
		t.Fatalf("WeightedSum failed: %v", err)
	}
	if !almostEqual(sum, 0.0, tolerance) {
		t.Errorf("WeightedSum = %v, expected 0.0", sum)
	}
}

func TestWeightedSumInvalidInputs(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 1.0}

	if _, err := WeightedSum(nil, nil); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for empty input, got %v", err)
	}
	if _, err := WeightedSum(data, weights[:2]); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for length mismatch, got %v", err)
	}
	if _, err := WeightedSum(data, []float64{1.0, -1.0, 1.0}); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for negative weight, got %v", err)
	}
	if _, err := WeightedSum([]float64{1.0, math.NaN(), 3.0}, weights); err != ErrNaNInput {
		t.Errorf("expected ErrNaNInput for NaN data, got %v", err)
	}
	if _, err := WeightedSum(data, []float64{1.0, math.NaN(), 1.0}); err != ErrNaNInput {
		t.Errorf("expected ErrNaNInput for NaN weight, got %v", err)
	}
}

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

	sums, err := WeightedSumBatch(data, weights, 2, 3)
	if err != nil {
		t.Fatalf("WeightedSumBatch failed: %v", err)
	}
	if !almostEqual(sums[0], 9.0, tolerance) || !almostEqual(sums[1], 80.0, tolerance) {
		t.Errorf("sums = %v, expected [9.0 80.0]", sums)
	}

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

	if _, err := WeightedSumBatch(data, weights, 0, 3); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for zero groups, got %v", err)
	}
	if _, err := WeightedSumBatch(data, weights, 1, 0); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for zero group size, got %v", err)
	}
	if _, err := WeightedSumBatch(data, weights, 2, 3); err != ErrInvalidArg {
		t.Errorf("expected ErrInvalidArg for size mismatch, got %v", err)
	}

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

func TestWeightedMedianBasic(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	weights := []float64{1.0, 1.0, 1.0, 1.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 3.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianSkewed(t *testing.T) {
	values := []float64{100.0, 200.0, 300.0}
	weights := []float64{10.0, 1.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 100.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianTwoElements(t *testing.T) {
	values := []float64{10.0, 20.0}
	weights := []float64{3.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 10.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianSingleElement(t *testing.T) {
	values := []float64{42.0}
	weights := []float64{1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 42.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianUnsorted(t *testing.T) {
	values := []float64{5.0, 1.0, 3.0, 4.0, 2.0}
	weights := []float64{1.0, 1.0, 1.0, 1.0, 1.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 3.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianZeroWeights(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	weights := []float64{0.0, 0.0, 1.0, 0.0, 0.0}

	result, err := WeightedMedian(values, weights)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 3.0
	if math.Abs(result-expected) > 1e-10 {
		t.Errorf("Expected %f, got %f", expected, result)
	}
}

func TestWeightedMedianErrorZeroSize(t *testing.T) {
	values := []float64{}
	weights := []float64{}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for zero size input, got nil")
	}
}

func TestWeightedMedianErrorMismatchedSizes(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for mismatched sizes, got nil")
	}
}

func TestWeightedMedianErrorAllWeightsZero(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{0.0, 0.0, 0.0}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for all weights zero, got nil")
	}
}

func TestWeightedMedianErrorNegativeWeight(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, -1.0, 1.0}

	_, err := WeightedMedian(values, weights)
	if err == nil {
		t.Errorf("Expected error for negative weight, got nil")
	}
}

func TestWeightedMedianCryptoScenario(t *testing.T) {
	exchangePrices := []float64{50000.0, 50050.0, 49900.0, 50020.0, 51000.0}
	volumes := []float64{100.0, 150.0, 120.0, 130.0, 10.0}

	result, err := WeightedMedian(exchangePrices, volumes)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	t.Logf("Aggregated price (volume-weighted median): %.2f", result)

	if result < 49000.0 || result > 52000.0 {
		t.Errorf("Result out of expected range: %f", result)
	}
}

func TestWeightedMedianBatchBasic(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0, 10.0, 20.0}
	weights := []float64{1.0, 1.0, 1.0, 3.0, 1.0}
	offsets := []int{0, 3}
	sizes := []int{3, 2}

	results, err := WeightedMedianBatch(values, weights, offsets, sizes)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	if len(results) != 2 {
		t.Fatalf("Expected 2 results, got %d", len(results))
	}

	if math.Abs(results[0]-2.0) > 1e-10 {
		t.Errorf("First batch: expected 2.0, got %f", results[0])
	}
	if math.Abs(results[1]-10.0) > 1e-10 {
		t.Errorf("Second batch: expected 10.0, got %f", results[1])
	}
}

func TestWeightedMedianBatchErrorZeroSize(t *testing.T) {
	values := []float64{1.0, 2.0}
	weights := []float64{1.0, 1.0}
	offsets := []int{}
	sizes := []int{}

	_, err := WeightedMedianBatch(values, weights, offsets, sizes)
	if err == nil {
		t.Errorf("Expected error for zero batch size, got nil")
	}
}

func TestWeightedMedianBatchErrorMismatchedSizes(t *testing.T) {
	values := []float64{1.0, 2.0, 3.0}
	weights := []float64{1.0, 1.0, 1.0}
	offsets := []int{0, 2}
	sizes := []int{2}

	_, err := WeightedMedianBatch(values, weights, offsets, sizes)
	if err == nil {
		t.Errorf("Expected error for mismatched batch sizes, got nil")
	}
}

func BenchmarkWeightedMedianSmall(b *testing.B) {
	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	weights := []float64{1.0, 1.0, 1.0, 1.0, 1.0}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedian(values, weights)
	}
}

func BenchmarkWeightedMedianMedium(b *testing.B) {
	values := make([]float64, 50)
	weights := make([]float64, 50)
	for i := 0; i < 50; i++ {
		values[i] = float64(i + 1)
		weights[i] = 1.0
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedian(values, weights)
	}
}

func BenchmarkWeightedMedianLarge(b *testing.B) {
	values := make([]float64, 1000)
	weights := make([]float64, 1000)
	for i := 0; i < 1000; i++ {
		values[i] = float64(i + 1)
		weights[i] = 1.0
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedian(values, weights)
	}
}

func BenchmarkWeightedMedianBatch1000Contracts(b *testing.B) {
	numContracts := 1000
	exchangesPerContract := 5
	totalSize := numContracts * exchangesPerContract

	values := make([]float64, totalSize)
	weights := make([]float64, totalSize)
	offsets := make([]int, numContracts)
	sizes := make([]int, numContracts)

	for i := 0; i < totalSize; i++ {
		values[i] = 50000.0 + float64(i%10000)
		weights[i] = 1.0 + float64(i%100)
	}
	for i := 0; i < numContracts; i++ {
		offsets[i] = i * exchangesPerContract
		sizes[i] = exchangesPerContract
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedianBatch(values, weights, offsets, sizes)
	}
}

func BenchmarkWeightedMedianBatch10000Contracts(b *testing.B) {
	numContracts := 10000
	exchangesPerContract := 5
	totalSize := numContracts * exchangesPerContract

	values := make([]float64, totalSize)
	weights := make([]float64, totalSize)
	offsets := make([]int, numContracts)
	sizes := make([]int, numContracts)

	for i := 0; i < totalSize; i++ {
		values[i] = 50000.0 + float64(i%10000)
		weights[i] = 1.0 + float64(i%100)
	}
	for i := 0; i < numContracts; i++ {
		offsets[i] = i * exchangesPerContract
		sizes[i] = exchangesPerContract
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = WeightedMedianBatch(values, weights, offsets, sizes)
	}
}

