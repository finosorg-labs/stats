package stats

import (
	"math"
	"testing"
)

func TestMomentsStateInit(t *testing.T) {
	m := NewMomentsState()
	if m.Count() != 0 {
		t.Errorf("Expected count 0, got %d", m.Count())
	}
	if m.Mean() != 0.0 {
		t.Errorf("Expected mean 0.0, got %f", m.Mean())
	}
}

func TestMomentsStateUpdate(t *testing.T) {
	m := NewMomentsState()

	m.Update(1.0)
	if m.Count() != 1 {
		t.Errorf("Expected count 1, got %d", m.Count())
	}
	if math.Abs(m.Mean()-1.0) > 1e-10 {
		t.Errorf("Expected mean 1.0, got %f", m.Mean())
	}

	m.Update(2.0)
	if m.Count() != 2 {
		t.Errorf("Expected count 2, got %d", m.Count())
	}
	if math.Abs(m.Mean()-1.5) > 1e-10 {
		t.Errorf("Expected mean 1.5, got %f", m.Mean())
	}

	m.Update(3.0)
	if m.Count() != 3 {
		t.Errorf("Expected count 3, got %d", m.Count())
	}
	if math.Abs(m.Mean()-2.0) > 1e-10 {
		t.Errorf("Expected mean 2.0, got %f", m.Mean())
	}
}

func TestMomentsStateUpdateBatch(t *testing.T) {
	m := NewMomentsState()
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}

	m.UpdateBatch(data)
	if m.Count() != 5 {
		t.Errorf("Expected count 5, got %d", m.Count())
	}
	if math.Abs(m.Mean()-3.0) > 1e-10 {
		t.Errorf("Expected mean 3.0, got %f", m.Mean())
	}
}

func TestMomentsSkewnessSymmetric(t *testing.T) {
	m := NewMomentsState()
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}

	m.UpdateBatch(data)
	skew := m.Skewness()

	if math.Abs(skew) > 0.1 {
		t.Errorf("Expected near-zero skewness for symmetric data, got %f", skew)
	}
}

func TestMomentsSkewnessRightSkewed(t *testing.T) {
	m := NewMomentsState()
	data := []float64{1.0, 1.0, 1.0, 2.0, 10.0}

	m.UpdateBatch(data)
	skew := m.Skewness()

	if skew <= 0.5 {
		t.Errorf("Expected positive skewness > 0.5 for right-skewed data, got %f", skew)
	}
}

func TestMomentsSkewnessLeftSkewed(t *testing.T) {
	m := NewMomentsState()
	data := []float64{1.0, 9.0, 10.0, 10.0, 10.0}

	m.UpdateBatch(data)
	skew := m.Skewness()

	if skew >= -0.5 {
		t.Errorf("Expected negative skewness < -0.5 for left-skewed data, got %f", skew)
	}
}

func TestMomentsKurtosis(t *testing.T) {
	m := NewMomentsState()
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0}

	m.UpdateBatch(data)
	kurt := m.Kurtosis()
	excessKurt := m.ExcessKurtosis()

	if !math.IsInf(kurt, 0) && math.IsNaN(kurt) {
		t.Errorf("Expected finite kurtosis, got %f", kurt)
	}

	if math.Abs(excessKurt-(kurt-3.0)) > 1e-10 {
		t.Errorf("Expected excess kurtosis = kurtosis - 3, got excess=%f, kurt=%f", excessKurt, kurt)
	}
}

func TestMomentsInsufficientData(t *testing.T) {
	m := NewMomentsState()

	skew := m.Skewness()
	if !math.IsNaN(skew) {
		t.Errorf("Expected NaN skewness with no data, got %f", skew)
	}

	m.Update(1.0)
	skew = m.Skewness()
	if !math.IsNaN(skew) {
		t.Errorf("Expected NaN skewness with 1 sample, got %f", skew)
	}

	m.Update(2.0)
	skew = m.Skewness()
	if !math.IsNaN(skew) {
		t.Errorf("Expected NaN skewness with 2 samples, got %f", skew)
	}

	kurt := m.Kurtosis()
	if !math.IsNaN(kurt) {
		t.Errorf("Expected NaN kurtosis with 2 samples, got %f", kurt)
	}

	m.Update(3.0)
	skew = m.Skewness()
	if math.IsNaN(skew) {
		t.Errorf("Expected finite skewness with 3 samples, got NaN")
	}

	kurt = m.Kurtosis()
	if !math.IsNaN(kurt) {
		t.Errorf("Expected NaN kurtosis with 3 samples, got %f", kurt)
	}

	m.Update(4.0)
	kurt = m.Kurtosis()
	if math.IsNaN(kurt) {
		t.Errorf("Expected finite kurtosis with 4 samples, got NaN")
	}
}

func TestMomentsConstantData(t *testing.T) {
	m := NewMomentsState()
	data := []float64{5.0, 5.0, 5.0, 5.0, 5.0}

	m.UpdateBatch(data)
	skew := m.Skewness()
	kurt := m.Kurtosis()

	if !math.IsNaN(skew) {
		t.Errorf("Expected NaN skewness for constant data, got %f", skew)
	}
	if !math.IsNaN(kurt) {
		t.Errorf("Expected NaN kurtosis for constant data, got %f", kurt)
	}
}

func TestBatchSkewness(t *testing.T) {
	data := []float64{
		1.0, 2.0, 3.0, 4.0, 5.0,
		1.0, 1.0, 1.0, 2.0, 10.0,
		1.0, 9.0, 10.0, 10.0, 10.0,
	}

	out, err := Skewness(data, 3, 5)
	if err != nil {
		t.Fatalf("Skewness failed: %v", err)
	}

	if len(out) != 3 {
		t.Fatalf("Expected 3 results, got %d", len(out))
	}

	if math.Abs(out[0]) > 0.1 {
		t.Errorf("Expected near-zero skewness for group 0, got %f", out[0])
	}
	if out[1] <= 0.5 {
		t.Errorf("Expected positive skewness > 0.5 for group 1, got %f", out[1])
	}
	if out[2] >= -0.5 {
		t.Errorf("Expected negative skewness < -0.5 for group 2, got %f", out[2])
	}
}

func TestBatchKurtosis(t *testing.T) {
	data := []float64{
		1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
		1.0, 1.0, 1.0, 10.0, 10.0, 10.0,
	}

	out, err := Kurtosis(data, 2, 6)
	if err != nil {
		t.Fatalf("Kurtosis failed: %v", err)
	}

	if len(out) != 2 {
		t.Fatalf("Expected 2 results, got %d", len(out))
	}

	if math.IsNaN(out[0]) || math.IsInf(out[0], 0) {
		t.Errorf("Expected finite kurtosis for group 0, got %f", out[0])
	}
	if math.IsNaN(out[1]) || math.IsInf(out[1], 0) {
		t.Errorf("Expected finite kurtosis for group 1, got %f", out[1])
	}
}

func TestBatchExcessKurtosis(t *testing.T) {
	data := []float64{
		1.0, 2.0, 3.0, 4.0, 5.0, 6.0,
		1.0, 1.0, 1.0, 10.0, 10.0, 10.0,
	}

	outKurt, err := Kurtosis(data, 2, 6)
	if err != nil {
		t.Fatalf("Kurtosis failed: %v", err)
	}

	outExcess, err := ExcessKurtosis(data, 2, 6)
	if err != nil {
		t.Fatalf("ExcessKurtosis failed: %v", err)
	}

	for i := 0; i < 2; i++ {
		if math.Abs(outExcess[i]-(outKurt[i]-3.0)) > 1e-10 {
			t.Errorf("Group %d: excess kurtosis should equal kurtosis - 3, got excess=%f, kurt=%f",
				i, outExcess[i], outKurt[i])
		}
	}
}

func TestBatchInvalidArgs(t *testing.T) {
	data := make([]float64, 100)

	_, err := Skewness(data, 0, 5)
	if err == nil {
		t.Error("Expected error for zero groups")
	}

	_, err = Skewness(data, 10, 2)
	if err == nil {
		t.Error("Expected error for group size < 3")
	}

	_, err = Kurtosis(data, 10, 3)
	if err == nil {
		t.Error("Expected error for group size < 4")
	}

	_, err = Skewness(data, 100, 5)
	if err == nil {
		t.Error("Expected error for insufficient data")
	}
}

func BenchmarkMomentsUpdate(b *testing.B) {
	m := NewMomentsState()
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		m.Update(42.0)
	}
}

func BenchmarkMomentsUpdateBatch100(b *testing.B) {
	data := make([]float64, 100)
	for i := range data {
		data[i] = float64(i)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		m := NewMomentsState()
		m.UpdateBatch(data)
	}
}

func BenchmarkMomentsUpdateBatch1000(b *testing.B) {
	data := make([]float64, 1000)
	for i := range data {
		data[i] = float64(i)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		m := NewMomentsState()
		m.UpdateBatch(data)
	}
}

func BenchmarkSkewnessBatch100x100(b *testing.B) {
	data := make([]float64, 100*100)
	for i := range data {
		data[i] = float64(i % 100)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Skewness(data, 100, 100)
	}
}

func BenchmarkKurtosisBatch100x100(b *testing.B) {
	data := make([]float64, 100*100)
	for i := range data {
		data[i] = float64(i % 100)
	}
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Kurtosis(data, 100, 100)
	}
}
