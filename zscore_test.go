package stats

import (
	"math"
	"testing"
)

func TestZScoreBasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	result, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore failed: %v", err)
	}

	if len(result) != len(data) {
		t.Fatalf("Expected length %d, got %d", len(data), len(result))
	}

	mean := 0.0
	for _, v := range result {
		mean += v
	}
	mean /= float64(len(result))

	if math.Abs(mean) > 1e-10 {
		t.Errorf("Expected mean ~0, got %f", mean)
	}

	variance := 0.0
	for _, v := range result {
		variance += v * v
	}
	variance /= float64(len(result) - 1)
	std := math.Sqrt(variance)

	if math.Abs(std-1.0) > 1e-10 {
		t.Errorf("Expected std ~1, got %f", std)
	}

	if math.Abs(result[2]) > 1e-10 {
		t.Errorf("Expected middle value ~0, got %f", result[2])
	}
}

func TestZScorePopulationVsSample(t *testing.T) {
	data := []float64{10.0, 20.0, 30.0, 40.0}

	sample, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore (sample) failed: %v", err)
	}

	pop, err := ZScore(data, false)
	if err != nil {
		t.Fatalf("ZScore (population) failed: %v", err)
	}

	for i := range sample {
		if math.Abs(sample[i]) >= math.Abs(pop[i]) && math.Abs(sample[i]) > 1e-10 {
			t.Errorf("Sample std is larger, so z-scores should be smaller in absolute value")
		}
	}
}

func TestZScoreConstantData(t *testing.T) {
	data := []float64{5.0, 5.0, 5.0, 5.0, 5.0}
	result, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore failed: %v", err)
	}

	for i, v := range result {
		if !math.IsNaN(v) {
			t.Errorf("Expected NaN at index %d, got %f", i, v)
		}
	}
}

func TestZScoreSingleElement(t *testing.T) {
	data := []float64{42.0}
	result, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore failed: %v", err)
	}

	if len(result) != 1 {
		t.Fatalf("Expected length 1, got %d", len(result))
	}

	if math.Abs(result[0]) > 1e-10 {
		t.Errorf("Expected 0, got %f", result[0])
	}
}

func TestZScoreEmpty(t *testing.T) {
	data := []float64{}
	result, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore failed: %v", err)
	}

	if len(result) != 0 {
		t.Errorf("Expected empty result, got length %d", len(result))
	}
}

func TestZScoreInPlace(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	expected, _ := ZScore(data, true)

	dataCopy := make([]float64, len(data))
	copy(dataCopy, data)

	err := ZScoreInPlace(dataCopy, true)
	if err != nil {
		t.Fatalf("ZScoreInPlace failed: %v", err)
	}

	for i := range expected {
		if math.Abs(dataCopy[i]-expected[i]) > 1e-10 {
			t.Errorf("In-place result differs at index %d: expected %f, got %f",
				i, expected[i], dataCopy[i])
		}
	}
}

func TestZScoreNegativeValues(t *testing.T) {
	data := []float64{-10.0, -5.0, 0.0, 5.0, 10.0}
	result, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore failed: %v", err)
	}

	if math.Abs(result[2]) > 1e-10 {
		t.Errorf("Expected middle value ~0, got %f", result[2])
	}

	if result[0] >= 0 {
		t.Errorf("Expected negative z-score for negative value, got %f", result[0])
	}

	if result[4] <= 0 {
		t.Errorf("Expected positive z-score for positive value, got %f", result[4])
	}
}

func TestZScoreLargeValues(t *testing.T) {
	data := []float64{1e10, 2e10, 3e10, 4e10, 5e10}
	result, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore failed: %v", err)
	}

	if math.Abs(result[2]) > 1e-6 {
		t.Errorf("Expected middle value ~0, got %f", result[2])
	}
}

func TestZScoreBatchBasic(t *testing.T) {
	data := []float64{
		1.0, 2.0, 3.0, 4.0,
		10.0, 20.0, 30.0, 40.0,
		100.0, 200.0, 300.0, 400.0,
	}

	result, err := ZScoreBatch(data, 3, 4, true)
	if err != nil {
		t.Fatalf("ZScoreBatch failed: %v", err)
	}

	for g := 0; g < 3; g++ {
		mean := 0.0
		for i := 0; i < 4; i++ {
			mean += result[g*4+i]
		}
		mean /= 4.0

		if math.Abs(mean) > 1e-10 {
			t.Errorf("Group %d mean should be ~0, got %f", g, mean)
		}
	}
}

func TestZScoreBatchInvalidSize(t *testing.T) {
	data := []float64{1, 2, 3, 4, 5}
	_, err := ZScoreBatch(data, 2, 3, true)
	if err == nil {
		t.Error("Expected error for mismatched size, got nil")
	}
}

func TestZScoreBatchEmpty(t *testing.T) {
	data := []float64{}
	result, err := ZScoreBatch(data, 0, 4, true)
	if err != nil {
		t.Fatalf("ZScoreBatch failed: %v", err)
	}

	if len(result) != 0 {
		t.Errorf("Expected empty result, got length %d", len(result))
	}
}

func TestZScoreBatchInPlace(t *testing.T) {
	data := []float64{
		1.0, 2.0, 3.0, 4.0,
		10.0, 20.0, 30.0, 40.0,
	}

	expected, _ := ZScoreBatch(data, 2, 4, true)

	dataCopy := make([]float64, len(data))
	copy(dataCopy, data)

	err := ZScoreBatchInPlace(dataCopy, 2, 4, true)
	if err != nil {
		t.Fatalf("ZScoreBatchInPlace failed: %v", err)
	}

	for i := range expected {
		if math.Abs(dataCopy[i]-expected[i]) > 1e-10 {
			t.Errorf("In-place result differs at index %d: expected %f, got %f",
				i, expected[i], dataCopy[i])
		}
	}
}

func TestZScoreLargeDataset(t *testing.T) {
	n := 10000
	data := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(i)
	}

	result, err := ZScore(data, true)
	if err != nil {
		t.Fatalf("ZScore failed: %v", err)
	}

	if len(result) != n {
		t.Fatalf("Expected length %d, got %d", n, len(result))
	}

	if math.Abs(result[n/2]) > 1e-3 {
		t.Errorf("Expected middle value ~0, got %f", result[n/2])
	}
}

func BenchmarkZScore100(b *testing.B) {
	data := make([]float64, 100)
	for i := 0; i < 100; i++ {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = ZScore(data, true)
	}
}

func BenchmarkZScore1K(b *testing.B) {
	data := make([]float64, 1000)
	for i := 0; i < 1000; i++ {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = ZScore(data, true)
	}
}

func BenchmarkZScore10K(b *testing.B) {
	data := make([]float64, 10000)
	for i := 0; i < 10000; i++ {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = ZScore(data, true)
	}
}

func BenchmarkZScoreInPlace10K(b *testing.B) {
	data := make([]float64, 10000)
	for i := 0; i < 10000; i++ {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		dataCopy := make([]float64, len(data))
		copy(dataCopy, data)
		_ = ZScoreInPlace(dataCopy, true)
	}
}

func BenchmarkZScoreBatch5000x250(b *testing.B) {
	data := make([]float64, 5000*250)
	for i := 0; i < len(data); i++ {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = ZScoreBatch(data, 5000, 250, true)
	}
}
