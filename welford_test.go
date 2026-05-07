package stats

import (
	"math"
	"testing"
)

func TestWelfordNew(t *testing.T) {
	w, err := NewWelford()
	if err != nil {
		t.Fatalf("NewWelford failed: %v", err)
	}

	count, err := w.Count()
	if err != nil {
		t.Fatalf("Count failed: %v", err)
	}
	if count != 0 {
		t.Errorf("Expected count 0, got %d", count)
	}
}

func TestWelfordUpdateSingle(t *testing.T) {
	w, _ := NewWelford()

	if err := w.Update(5.0); err != nil {
		t.Fatalf("Update failed: %v", err)
	}

	count, _ := w.Count()
	if count != 1 {
		t.Errorf("Expected count 1, got %d", count)
	}

	mean, _ := w.Mean()
	if math.Abs(mean-5.0) > epsilon {
		t.Errorf("Expected mean 5.0, got %f", mean)
	}

	if err := w.Update(7.0); err != nil {
		t.Fatalf("Update failed: %v", err)
	}

	count, _ = w.Count()
	if count != 2 {
		t.Errorf("Expected count 2, got %d", count)
	}

	mean, _ = w.Mean()
	if math.Abs(mean-6.0) > epsilon {
		t.Errorf("Expected mean 6.0, got %f", mean)
	}
}

func TestWelfordUpdateNaN(t *testing.T) {
	w, _ := NewWelford()

	err := w.Update(math.NaN())
	if err != ErrNaNInput {
		t.Errorf("Expected ErrNaNInput, got %v", err)
	}

	count, _ := w.Count()
	if count != 0 {
		t.Errorf("Expected count 0 after NaN, got %d", count)
	}
}

func TestWelfordMean(t *testing.T) {
	w, _ := NewWelford()

	mean, err := w.Mean()
	if err != nil {
		t.Fatalf("Mean failed: %v", err)
	}
	if mean != 0.0 {
		t.Errorf("Expected mean 0.0 for empty state, got %f", mean)
	}

	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	for _, v := range values {
		w.Update(v)
	}

	mean, err = w.Mean()
	if err != nil {
		t.Fatalf("Mean failed: %v", err)
	}
	if math.Abs(mean-3.0) > epsilon {
		t.Errorf("Expected mean 3.0, got %f", mean)
	}
}

func TestWelfordVarianceSample(t *testing.T) {
	w, _ := NewWelford()

	variance, err := w.Variance(true)
	if err != nil {
		t.Fatalf("Variance failed: %v", err)
	}
	if variance != 0.0 {
		t.Errorf("Expected variance 0.0 for empty state, got %f", variance)
	}

	w.Update(1.0)
	variance, _ = w.Variance(true)
	if variance != 0.0 {
		t.Errorf("Expected variance 0.0 for single value, got %f", variance)
	}

	values := []float64{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0}
	w.Reset()
	for _, v := range values {
		w.Update(v)
	}

	variance, err = w.Variance(true)
	if err != nil {
		t.Fatalf("Variance failed: %v", err)
	}
	if math.Abs(variance-4.571428571428571) > epsilon {
		t.Errorf("Expected variance 4.571428571428571, got %f", variance)
	}
}

func TestWelfordVariancePopulation(t *testing.T) {
	w, _ := NewWelford()

	values := []float64{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0}
	for _, v := range values {
		w.Update(v)
	}

	variance, err := w.Variance(false)
	if err != nil {
		t.Fatalf("Variance failed: %v", err)
	}
	if math.Abs(variance-4.0) > epsilon {
		t.Errorf("Expected variance 4.0, got %f", variance)
	}
}

func TestWelfordStdDev(t *testing.T) {
	w, _ := NewWelford()

	values := []float64{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0}
	for _, v := range values {
		w.Update(v)
	}

	stddev, err := w.StdDev(true)
	if err != nil {
		t.Fatalf("StdDev failed: %v", err)
	}
	if math.Abs(stddev-2.138089935299395) > epsilon {
		t.Errorf("Expected stddev 2.138089935299395, got %f", stddev)
	}

	stddev, err = w.StdDev(false)
	if err != nil {
		t.Fatalf("StdDev failed: %v", err)
	}
	if math.Abs(stddev-2.0) > epsilon {
		t.Errorf("Expected stddev 2.0, got %f", stddev)
	}
}

func TestWelfordUpdateBatch(t *testing.T) {
	w, _ := NewWelford()

	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	if err := w.UpdateBatch(values); err != nil {
		t.Fatalf("UpdateBatch failed: %v", err)
	}

	count, _ := w.Count()
	if count != 5 {
		t.Errorf("Expected count 5, got %d", count)
	}

	mean, _ := w.Mean()
	if math.Abs(mean-3.0) > epsilon {
		t.Errorf("Expected mean 3.0, got %f", mean)
	}

	variance, _ := w.Variance(true)
	if math.Abs(variance-2.5) > epsilon {
		t.Errorf("Expected variance 2.5, got %f", variance)
	}
}

func TestWelfordUpdateBatchEmpty(t *testing.T) {
	w, _ := NewWelford()

	err := w.UpdateBatch([]float64{})
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg for empty batch, got %v", err)
	}
}

func TestWelfordUpdateBatchNaN(t *testing.T) {
	w, _ := NewWelford()

	values := []float64{1.0, 2.0, math.NaN(), 4.0, 5.0}
	err := w.UpdateBatch(values)
	if err != ErrNaNInput {
		t.Errorf("Expected ErrNaNInput, got %v", err)
	}
}

func TestWelfordMergeEmpty(t *testing.T) {
	w1, _ := NewWelford()
	w2, _ := NewWelford()

	if err := w1.Merge(w2); err != nil {
		t.Fatalf("Merge failed: %v", err)
	}

	count, _ := w1.Count()
	if count != 0 {
		t.Errorf("Expected count 0, got %d", count)
	}

	w1.Update(5.0)
	if err := w1.Merge(w2); err != nil {
		t.Fatalf("Merge failed: %v", err)
	}

	count, _ = w1.Count()
	if count != 1 {
		t.Errorf("Expected count 1, got %d", count)
	}

	mean, _ := w1.Mean()
	if math.Abs(mean-5.0) > epsilon {
		t.Errorf("Expected mean 5.0, got %f", mean)
	}
}

func TestWelfordMergeBasic(t *testing.T) {
	w1, _ := NewWelford()
	w2, _ := NewWelford()

	values1 := []float64{1.0, 2.0, 3.0}
	values2 := []float64{4.0, 5.0, 6.0}

	w1.UpdateBatch(values1)
	w2.UpdateBatch(values2)

	if err := w1.Merge(w2); err != nil {
		t.Fatalf("Merge failed: %v", err)
	}

	count, _ := w1.Count()
	if count != 6 {
		t.Errorf("Expected count 6, got %d", count)
	}

	mean, _ := w1.Mean()
	if math.Abs(mean-3.5) > epsilon {
		t.Errorf("Expected mean 3.5, got %f", mean)
	}

	variance, _ := w1.Variance(true)
	if math.Abs(variance-3.5) > epsilon {
		t.Errorf("Expected variance 3.5, got %f", variance)
	}
}

func TestWelfordMergeAsymmetric(t *testing.T) {
	w1, _ := NewWelford()
	w2, _ := NewWelford()

	values1 := []float64{1.0, 2.0}
	values2 := []float64{3.0, 4.0, 5.0, 6.0}

	w1.UpdateBatch(values1)
	w2.UpdateBatch(values2)

	if err := w1.Merge(w2); err != nil {
		t.Fatalf("Merge failed: %v", err)
	}

	count, _ := w1.Count()
	if count != 6 {
		t.Errorf("Expected count 6, got %d", count)
	}

	mean, _ := w1.Mean()
	if math.Abs(mean-3.5) > epsilon {
		t.Errorf("Expected mean 3.5, got %f", mean)
	}
}

func TestWelfordMergeNil(t *testing.T) {
	w, _ := NewWelford()

	err := w.Merge(nil)
	if err != ErrInvalidArg {
		t.Errorf("Expected ErrInvalidArg for nil merge, got %v", err)
	}
}

func TestWelfordReset(t *testing.T) {
	w, _ := NewWelford()

	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	w.UpdateBatch(values)

	count, _ := w.Count()
	if count != 5 {
		t.Errorf("Expected count 5 before reset, got %d", count)
	}

	if err := w.Reset(); err != nil {
		t.Fatalf("Reset failed: %v", err)
	}

	count, _ = w.Count()
	if count != 0 {
		t.Errorf("Expected count 0 after reset, got %d", count)
	}

	mean, _ := w.Mean()
	if mean != 0.0 {
		t.Errorf("Expected mean 0.0 after reset, got %f", mean)
	}
}

func TestWelfordNumericalStability(t *testing.T) {
	w, _ := NewWelford()

	largeBase := 1e9
	values := []float64{largeBase + 1.0, largeBase + 2.0, largeBase + 3.0}

	for _, v := range values {
		w.Update(v)
	}

	mean, _ := w.Mean()
	if math.Abs(mean-(largeBase+2.0)) > epsilon {
		t.Errorf("Expected mean %f, got %f", largeBase+2.0, mean)
	}

	variance, _ := w.Variance(true)
	if math.Abs(variance-1.0) > epsilon {
		t.Errorf("Expected variance 1.0, got %f", variance)
	}
}

func TestWelfordLargeDataset(t *testing.T) {
	w, _ := NewWelford()

	n := 10000
	for i := 0; i < n; i++ {
		w.Update(float64(i))
	}

	mean, _ := w.Mean()
	expectedMean := 4999.5
	if math.Abs(mean-expectedMean) > 1e-6 {
		t.Errorf("Expected mean %f, got %f", expectedMean, mean)
	}

	expectedVariance := float64(n*n-1) / 12.0
	variance, _ := w.Variance(true)
	if math.Abs(variance-expectedVariance) > 1000.0 {
		t.Errorf("Expected variance ~%f, got %f", expectedVariance, variance)
	}
}

func BenchmarkWelfordUpdate(b *testing.B) {
	w, _ := NewWelford()

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		w.Update(float64(i))
	}
}

func BenchmarkWelfordUpdateBatch(b *testing.B) {
	values := make([]float64, 1000)
	for i := range values {
		values[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		w, _ := NewWelford()
		w.UpdateBatch(values)
	}
}

func BenchmarkWelfordMerge(b *testing.B) {
	w1, _ := NewWelford()
	w2, _ := NewWelford()

	for i := 0; i < 1000; i++ {
		w1.Update(float64(i))
		w2.Update(float64(i + 1000))
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		w1Copy := *w1
		w1Copy.Merge(w2)
	}
}

func TestUpdateStreams_Basic(t *testing.T) {
	// Create 4 independent streams
	states := make([]*WelfordState, 4)
	for i := range states {
		states[i], _ = NewWelford()
	}

	// Update each stream with 3 values
	values1 := []float64{1.0, 2.0, 3.0, 4.0}
	values2 := []float64{5.0, 6.0, 7.0, 8.0}
	values3 := []float64{9.0, 10.0, 11.0, 12.0}

	if err := UpdateStreams(states, values1); err != nil {
		t.Fatalf("UpdateStreams failed: %v", err)
	}
	if err := UpdateStreams(states, values2); err != nil {
		t.Fatalf("UpdateStreams failed: %v", err)
	}
	if err := UpdateStreams(states, values3); err != nil {
		t.Fatalf("UpdateStreams failed: %v", err)
	}

	// Verify each stream
	expectedMeans := []float64{5.0, 6.0, 7.0, 8.0} // (1+5+9)/3, (2+6+10)/3, etc.
	for i, state := range states {
		mean, err := state.Mean()
		if err != nil {
			t.Fatalf("Mean failed for stream %d: %v", i, err)
		}
		if math.Abs(mean-expectedMeans[i]) > 1e-9 {
			t.Errorf("Stream %d: mean = %f, want %f", i, mean, expectedMeans[i])
		}

		count, err := state.Count()
		if err != nil {
			t.Fatalf("Count failed for stream %d: %v", i, err)
		}
		if count != 3 {
			t.Errorf("Stream %d: count = %d, want 3", i, count)
		}
	}
}

func TestUpdateStreams_Large(t *testing.T) {
	// Test 100 streams (realistic scenario)
	const nStreams = 100
	states := make([]*WelfordState, nStreams)
	for i := range states {
		states[i], _ = NewWelford()
	}

	// Update 10 times
	for iter := 0; iter < 10; iter++ {
		values := make([]float64, nStreams)
		for i := range values {
			values[i] = float64(iter*100 + i)
		}
		if err := UpdateStreams(states, values); err != nil {
			t.Fatalf("UpdateStreams failed at iteration %d: %v", iter, err)
		}
	}

	// Verify counts
	for i, state := range states {
		count, err := state.Count()
		if err != nil {
			t.Fatalf("Count failed for stream %d: %v", i, err)
		}
		if count != 10 {
			t.Errorf("Stream %d: count = %d, want 10", i, count)
		}
	}
}

func TestUpdateStreams_VsSingle(t *testing.T) {
	// Verify that updating streams gives same result as updating individually
	statesStream := make([]*WelfordState, 4)
	statesSingle := make([]*WelfordState, 4)
	for i := range statesStream {
		statesStream[i], _ = NewWelford()
		statesSingle[i], _ = NewWelford()
	}

	// Update using streams
	vals1 := []float64{1.0, 2.0, 3.0, 4.0}
	vals2 := []float64{5.0, 6.0, 7.0, 8.0}
	vals3 := []float64{9.0, 10.0, 11.0, 12.0}

	UpdateStreams(statesStream, vals1)
	UpdateStreams(statesStream, vals2)
	UpdateStreams(statesStream, vals3)

	// Update individually
	statesSingle[0].Update(1.0)
	statesSingle[0].Update(5.0)
	statesSingle[0].Update(9.0)

	statesSingle[1].Update(2.0)
	statesSingle[1].Update(6.0)
	statesSingle[1].Update(10.0)

	statesSingle[2].Update(3.0)
	statesSingle[2].Update(7.0)
	statesSingle[2].Update(11.0)

	statesSingle[3].Update(4.0)
	statesSingle[3].Update(8.0)
	statesSingle[3].Update(12.0)

	// Compare results
	for i := range statesStream {
		meanStream, _ := statesStream[i].Mean()
		meanSingle, _ := statesSingle[i].Mean()
		if math.Abs(meanStream-meanSingle) > 1e-9 {
			t.Errorf("Stream %d: mean mismatch: stream=%f, single=%f", i, meanStream, meanSingle)
		}

		varStream, _ := statesStream[i].Variance(true)
		varSingle, _ := statesSingle[i].Variance(true)
		if math.Abs(varStream-varSingle) > 1e-9 {
			t.Errorf("Stream %d: variance mismatch: stream=%f, single=%f", i, varStream, varSingle)
		}
	}
}

func BenchmarkUpdateStreams_SSE(b *testing.B) {
	states := make([]*WelfordState, 2)
	for i := range states {
		states[i], _ = NewWelford()
	}
	values := []float64{1.0, 2.0}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		UpdateStreams(states, values)
	}
}

func BenchmarkUpdateStreams_AVX2(b *testing.B) {
	states := make([]*WelfordState, 4)
	for i := range states {
		states[i], _ = NewWelford()
	}
	values := []float64{1.0, 2.0, 3.0, 4.0}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		UpdateStreams(states, values)
	}
}

func BenchmarkUpdateStreams_AVX512(b *testing.B) {
	states := make([]*WelfordState, 8)
	for i := range states {
		states[i], _ = NewWelford()
	}
	values := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		UpdateStreams(states, values)
	}
}

func BenchmarkUpdateStreams_Large(b *testing.B) {
	const nStreams = 1000
	states := make([]*WelfordState, nStreams)
	for i := range states {
		states[i], _ = NewWelford()
	}
	values := make([]float64, nStreams)
	for i := range values {
		values[i] = float64(i)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		UpdateStreams(states, values)
	}
}
