package stats

import (
	"math"
	"testing"
)

// Helper function to compute expected EMA
func computeExpectedEMA(data []float64, alpha float64, init float64) []float64 {
	oneMinusAlpha := 1.0 - alpha
	currentEMA := init
	result := make([]float64, len(data))

	for i := 0; i < len(data); i++ {
		currentEMA = alpha*data[i] + oneMinusAlpha*currentEMA
		result[i] = currentEMA
	}

	return result
}

func TestEMABasic(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	alpha := 0.5

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], expected[i])
		}
	}
}

func TestEMAWithInit(t *testing.T) {
	data := []float64{10.0, 20.0, 30.0, 40.0, 50.0}
	alpha := 0.3
	initValue := 5.0

	ema, err := EMAWithInit(data, alpha, initValue)
	if err != nil {
		t.Fatalf("EMAWithInit failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, initValue)

	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], expected[i])
		}
	}
}

func TestEMAAlphaOne(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	alpha := 1.0

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	// With alpha = 1.0, EMA should equal input
	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], data[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], data[i])
		}
	}
}

func TestEMASmallAlpha(t *testing.T) {
	data := []float64{100.0, 100.0, 100.0, 100.0, 100.0}
	alpha := 0.1

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	// With constant input, EMA should converge to input value
	if !almostEqual(ema[len(ema)-1], 100.0, epsilon) {
		t.Errorf("Final EMA = %f, expected 100.0", ema[len(ema)-1])
	}
}

func TestEMAFinancialAlpha(t *testing.T) {
	data := []float64{100.0, 102.0, 101.0, 103.0, 105.0, 104.0, 106.0, 108.0}
	period := 5
	alpha := 2.0 / float64(period+1) // 0.333...

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], expected[i])
		}
	}
}

func TestEMAInPlace(t *testing.T) {
	data := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	alpha := 0.5

	expected := computeExpectedEMA(data, alpha, data[0])

	err := EMAInPlace(data, alpha)
	if err != nil {
		t.Fatalf("EMAInPlace failed: %v", err)
	}

	for i := 0; i < len(data); i++ {
		if !almostEqual(data[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, data[i], expected[i])
		}
	}
}

func TestEMABatch(t *testing.T) {
	data := []float64{
		1.0, 2.0, 3.0, 4.0, 5.0, // Series 1
		10.0, 20.0, 30.0, 40.0, 50.0, // Series 2
		100.0, 90.0, 80.0, 70.0, 60.0, // Series 3
	}
	numSeries := 3
	seriesLength := 5
	alpha := 0.4

	ema, err := EMABatch(data, numSeries, seriesLength, alpha)
	if err != nil {
		t.Fatalf("EMABatch failed: %v", err)
	}

	// Verify each series independently
	for series := 0; series < numSeries; series++ {
		seriesData := data[series*seriesLength : (series+1)*seriesLength]
		expected := computeExpectedEMA(seriesData, alpha, seriesData[0])

		for i := 0; i < seriesLength; i++ {
			idx := series*seriesLength + i
			if !almostEqual(ema[idx], expected[i], epsilon) {
				t.Errorf("Series %d, EMA[%d] = %f, expected %f", series, i, ema[idx], expected[i])
			}
		}
	}
}

func TestEMAPeriod(t *testing.T) {
	data := []float64{100.0, 102.0, 101.0, 103.0, 105.0}
	period := 5

	ema, err := EMAPeriod(data, period)
	if err != nil {
		t.Fatalf("EMAPeriod failed: %v", err)
	}

	// Verify it's equivalent to EMA with calculated alpha
	alpha := 2.0 / float64(period+1)
	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], expected[i])
		}
	}
}

func TestEMALarge(t *testing.T) {
	n := 10000
	data := make([]float64, n)
	alpha := 0.2

	// Generate test data
	for i := 0; i < n; i++ {
		data[i] = math.Sin(float64(i)*0.01)*100.0 + 100.0
	}

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	// Verify first few values
	expected := computeExpectedEMA(data[:10], alpha, data[0])

	for i := 0; i < 10; i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], expected[i])
		}
	}
}

func TestEMAErrorHandling(t *testing.T) {
	tests := []struct {
		name  string
		data  []float64
		alpha float64
		want  error
	}{
		{"empty data", []float64{}, 0.5, ErrInvalidArg},
		{"alpha zero", []float64{1, 2, 3}, 0.0, ErrInvalidArg},
		{"alpha negative", []float64{1, 2, 3}, -0.1, ErrInvalidArg},
		{"alpha too large", []float64{1, 2, 3}, 1.5, ErrInvalidArg},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, err := EMA(tt.data, tt.alpha)
			if err != tt.want {
				t.Errorf("EMA() error = %v, want %v", err, tt.want)
			}
		})
	}
}

func TestEMASingleElement(t *testing.T) {
	data := []float64{42.0}
	alpha := 0.5

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	if !almostEqual(ema[0], 42.0, epsilon) {
		t.Errorf("EMA[0] = %f, expected 42.0", ema[0])
	}
}

func TestEMANegativeValues(t *testing.T) {
	data := []float64{-10.0, -5.0, 0.0, 5.0, 10.0}
	alpha := 0.3

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], expected[i])
		}
	}
}

// Benchmark tests

func BenchmarkEMA100(b *testing.B) {
	data := make([]float64, 100)
	for i := range data {
		data[i] = float64(i)
	}
	alpha := 0.2

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = EMA(data, alpha)
	}
}

func BenchmarkEMA1000(b *testing.B) {
	data := make([]float64, 1000)
	for i := range data {
		data[i] = float64(i)
	}
	alpha := 0.2

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = EMA(data, alpha)
	}
}

func BenchmarkEMA10000(b *testing.B) {
	data := make([]float64, 10000)
	for i := range data {
		data[i] = float64(i)
	}
	alpha := 0.2

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = EMA(data, alpha)
	}
}

func BenchmarkEMAInPlace10000(b *testing.B) {
	original := make([]float64, 10000)
	for i := range original {
		original[i] = float64(i)
	}
	data := make([]float64, 10000)
	alpha := 0.2

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		copy(data, original)
		_ = EMAInPlace(data, alpha)
	}
}

func BenchmarkEMABatch(b *testing.B) {
	numSeries := 1000
	seriesLength := 250
	data := make([]float64, numSeries*seriesLength)
	for i := range data {
		data[i] = float64(i % seriesLength)
	}
	alpha := 0.2

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = EMABatch(data, numSeries, seriesLength, alpha)
	}
}

// Additional test cases for edge cases and special values

func TestEMAVerySmallAlpha(t *testing.T) {
	data := []float64{100.0, 110.0, 120.0, 130.0, 140.0}
	alpha := 0.001 // Very small alpha - slow adaptation

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			t.Errorf("EMA[%d] = %f, expected %f", i, ema[i], expected[i])
		}
	}

	// With very small alpha, EMA should change slowly
	if ema[len(ema)-1] > 105.0 {
		t.Errorf("EMA adapted too quickly with small alpha: final = %f", ema[len(ema)-1])
	}
}

func TestEMANaNPropagation(t *testing.T) {
	data := []float64{1.0, 2.0, math.NaN(), 4.0, 5.0}
	alpha := 0.5

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	// NaN should propagate through the rest of the series
	for i := 2; i < len(ema); i++ {
		if !math.IsNaN(ema[i]) {
			t.Errorf("EMA[%d] = %f, expected NaN", i, ema[i])
		}
	}
}

func TestEMAInfPropagation(t *testing.T) {
	data := []float64{1.0, 2.0, math.Inf(1), 4.0, 5.0}
	alpha := 0.5

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	// After Inf is encountered, the result depends on the formula:
	// With formula: EMA[i] = EMA[i-1] + alpha * (data[i] - EMA[i-1])
	// When data[i] = Inf: Inf - EMA[i-1] = Inf, alpha * Inf = Inf, EMA[i-1] + Inf = Inf
	// When data[i] is finite after Inf: finite - Inf = -Inf, alpha * (-Inf) = -Inf, Inf + (-Inf) = NaN

	// So Inf at index 2 should propagate as Inf, then become NaN when finite values follow
	if !math.IsInf(ema[2], 1) {
		t.Errorf("EMA[2] = %f, expected +Inf", ema[2])
	}

	// After Inf, when finite values are encountered, result becomes NaN due to Inf - Inf
	for i := 3; i < len(ema); i++ {
		if !math.IsNaN(ema[i]) {
			t.Errorf("EMA[%d] = %f, expected NaN (Inf arithmetic produces NaN)", i, ema[i])
		}
	}
}

func TestEMABatchVariousSizes(t *testing.T) {
	// Test different batch sizes to verify SIMD dispatch logic
	testCases := []struct {
		name       string
		numSeries  int
		seriesLen  int
		shouldPass bool
	}{
		{"1 series", 1, 10, true},
		{"2 series (SSE)", 2, 10, true},
		{"3 series", 3, 10, true},
		{"4 series (AVX2)", 4, 10, true},
		{"5 series", 5, 10, true},
		{"7 series", 7, 10, true},
		{"8 series (AVX512)", 8, 10, true},
		{"9 series", 9, 10, true},
		{"16 series", 16, 10, true},
	}

	alpha := 0.3

	for _, tc := range testCases {
		t.Run(tc.name, func(t *testing.T) {
			data := make([]float64, tc.numSeries*tc.seriesLen)
			for i := range data {
				data[i] = float64(i%tc.seriesLen + 1)
			}

			ema, err := EMABatch(data, tc.numSeries, tc.seriesLen, alpha)
			if err != nil {
				t.Fatalf("EMABatch failed: %v", err)
			}

			// Verify each series independently
			for series := 0; series < tc.numSeries; series++ {
				seriesData := data[series*tc.seriesLen : (series+1)*tc.seriesLen]
				expected := computeExpectedEMA(seriesData, alpha, seriesData[0])

				for i := 0; i < tc.seriesLen; i++ {
					idx := series*tc.seriesLen + i
					if !almostEqual(ema[idx], expected[i], epsilon) {
						t.Errorf("Series %d, EMA[%d] = %f, expected %f", series, i, ema[idx], expected[i])
					}
				}
			}
		})
	}
}

func TestEMANumericalStability(t *testing.T) {
	// Test with alpha very close to 1.0 to verify numerical stability
	data := []float64{1e10, 1e10 + 1, 1e10 + 2, 1e10 + 3, 1e10 + 4}
	alpha := 0.9999 // Very close to 1.0

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		// Use relative error for large numbers
		relErr := math.Abs(ema[i]-expected[i]) / math.Abs(expected[i])
		if relErr > 1e-10 {
			t.Errorf("EMA[%d] = %.15f, expected %.15f, rel_err = %e", i, ema[i], expected[i], relErr)
		}
	}
}

func TestEMALargeValues(t *testing.T) {
	data := []float64{1e15, 2e15, 3e15, 4e15, 5e15}
	alpha := 0.5

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		relErr := math.Abs(ema[i]-expected[i]) / math.Abs(expected[i])
		if relErr > epsilon {
			t.Errorf("EMA[%d] = %.15e, expected %.15e", i, ema[i], expected[i])
		}
	}
}

func TestEMASmallValues(t *testing.T) {
	data := []float64{1e-15, 2e-15, 3e-15, 4e-15, 5e-15}
	alpha := 0.5

	ema, err := EMA(data, alpha)
	if err != nil {
		t.Fatalf("EMA failed: %v", err)
	}

	expected := computeExpectedEMA(data, alpha, data[0])

	for i := 0; i < len(ema); i++ {
		if math.Abs(ema[i]-expected[i]) > 1e-25 {
			t.Errorf("EMA[%d] = %.15e, expected %.15e", i, ema[i], expected[i])
		}
	}
}

func TestEMABatchInPlace(t *testing.T) {
	numSeries := 5
	seriesLength := 10
	data := make([]float64, numSeries*seriesLength)
	for i := range data {
		data[i] = float64(i%seriesLength + 1)
	}
	alpha := 0.4

	// Compute expected values
	expected := make([]float64, len(data))
	for series := 0; series < numSeries; series++ {
		seriesData := data[series*seriesLength : (series+1)*seriesLength]
		seriesExpected := computeExpectedEMA(seriesData, alpha, seriesData[0])
		copy(expected[series*seriesLength:(series+1)*seriesLength], seriesExpected)
	}

	// Compute in-place using EMABatch (which supports in-place operation)
	ema, err := EMABatch(data, numSeries, seriesLength, alpha)
	if err != nil {
		t.Fatalf("EMABatch failed: %v", err)
	}

	// Verify results
	for i := 0; i < len(ema); i++ {
		if !almostEqual(ema[i], expected[i], epsilon) {
			series := i / seriesLength
			idx := i % seriesLength
			t.Errorf("Series %d, EMA[%d] = %f, expected %f", series, idx, ema[i], expected[i])
		}
	}
}
