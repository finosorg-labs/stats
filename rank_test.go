package stats

import (
	"math"
	"testing"
)

func TestRankBasicAverage(t *testing.T) {
	data := []float64{3.0, 1.0, 2.0, 2.0, 5.0}
	expected := []float64{4.0, 1.0, 2.5, 2.5, 5.0}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankBasicMin(t *testing.T) {
	data := []float64{3.0, 1.0, 2.0, 2.0, 5.0}
	expected := []float64{4.0, 1.0, 2.0, 2.0, 5.0}

	ranks, err := Rank(data, RankMin)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankBasicMax(t *testing.T) {
	data := []float64{3.0, 1.0, 2.0, 2.0, 5.0}
	expected := []float64{4.0, 1.0, 3.0, 3.0, 5.0}

	ranks, err := Rank(data, RankMax)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankBasicFirst(t *testing.T) {
	data := []float64{3.0, 1.0, 2.0, 2.0, 5.0}
	expected := []float64{4.0, 1.0, 2.0, 3.0, 5.0}

	ranks, err := Rank(data, RankFirst)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankBasicDense(t *testing.T) {
	data := []float64{3.0, 1.0, 2.0, 2.0, 5.0}
	expected := []float64{3.0, 1.0, 2.0, 2.0, 4.0}

	ranks, err := Rank(data, RankDense)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankSingleElement(t *testing.T) {
	data := []float64{42.0}
	expected := []float64{1.0}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	if math.Abs(ranks[0]-expected[0]) > 1e-10 {
		t.Errorf("ranks[0] = %f, expected %f", ranks[0], expected[0])
	}
}

func TestRankAllEqual(t *testing.T) {
	data := []float64{5.0, 5.0, 5.0, 5.0}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-2.5) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected 2.5", i, ranks[i])
		}
	}
}

func TestRankSortedAscending(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	expected := []float64{1.0, 2.0, 3.0, 4.0, 5.0}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankSortedDescending(t *testing.T) {
	data := []float64{5.0, 4.0, 3.0, 2.0, 1.0}
	expected := []float64{5.0, 4.0, 3.0, 2.0, 1.0}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankWithNaN(t *testing.T) {
	data := []float64{3.0, math.NaN(), 1.0, 2.0, math.NaN()}
	expected := []float64{3.0, 4.5, 1.0, 2.0, 4.5}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankNormalizedBasic(t *testing.T) {
	data := []float64{3.0, 1.0, 2.0, 2.0, 5.0}
	expected := []float64{0.75, 0.0, 0.375, 0.375, 1.0}

	ranks, err := RankNormalized(data, RankAverage)
	if err != nil {
		t.Fatalf("RankNormalized failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankNormalizedSingle(t *testing.T) {
	data := []float64{42.0}
	expected := []float64{0.5}

	ranks, err := RankNormalized(data, RankAverage)
	if err != nil {
		t.Fatalf("RankNormalized failed: %v", err)
	}

	if math.Abs(ranks[0]-expected[0]) > 1e-10 {
		t.Errorf("ranks[0] = %f, expected %f", ranks[0], expected[0])
	}
}

func TestRankNormalizedTwoElements(t *testing.T) {
	data := []float64{1.0, 2.0}
	expected := []float64{0.0, 1.0}

	ranks, err := RankNormalized(data, RankAverage)
	if err != nil {
		t.Fatalf("RankNormalized failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankBatchBasic(t *testing.T) {
	data := []float64{
		3.0, 1.0, 2.0, 2.0, 5.0,
		10.0, 20.0, 15.0, 15.0, 5.0,
	}
	expected := []float64{
		4.0, 1.0, 2.5, 2.5, 5.0,
		2.0, 5.0, 3.5, 3.5, 1.0,
	}

	ranks, err := RankBatch(data, 5, RankAverage)
	if err != nil {
		t.Fatalf("RankBatch failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankEmptyInput(t *testing.T) {
	data := []float64{}
	_, err := Rank(data, RankAverage)
	if err == nil {
		t.Error("Expected error for empty input, got nil")
	}
}

func TestRankNegativeValues(t *testing.T) {
	data := []float64{-3.0, -1.0, -2.0, 0.0, 1.0}
	expected := []float64{1.0, 3.0, 2.0, 4.0, 5.0}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := range ranks {
		if math.Abs(ranks[i]-expected[i]) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected[i])
		}
	}
}

func TestRankLargeArray(t *testing.T) {
	n := 10000
	data := make([]float64, n)
	for i := 0; i < n; i++ {
		data[i] = float64(n - i)
	}

	ranks, err := Rank(data, RankAverage)
	if err != nil {
		t.Fatalf("Rank failed: %v", err)
	}

	for i := 0; i < n; i++ {
		expected := float64(n - i)
		if math.Abs(ranks[i]-expected) > 1e-10 {
			t.Errorf("ranks[%d] = %f, expected %f", i, ranks[i], expected)
		}
	}
}

func BenchmarkRankSmall(b *testing.B) {
	data := make([]float64, 100)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Rank(data, RankAverage)
	}
}

func BenchmarkRankMedium(b *testing.B) {
	data := make([]float64, 1000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Rank(data, RankAverage)
	}
}

func BenchmarkRankLarge(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Rank(data, RankAverage)
	}
}

func BenchmarkRankCrossSection(b *testing.B) {
	data := make([]float64, 5000)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Rank(data, RankAverage)
	}
}

func BenchmarkRankBatch(b *testing.B) {
	n := 5000
	numFactors := 100
	data := make([]float64, n*numFactors)
	for i := range data {
		data[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = RankBatch(data, n, RankAverage)
	}
}
