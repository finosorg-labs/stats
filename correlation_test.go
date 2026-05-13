package stats

import (
	"math"
	"testing"
)

const testEpsilon = 1e-10

// Helper function to compare floats with tolerance
func floatEquals(a, b, epsilon float64) bool {
	return math.Abs(a-b) < epsilon
}

// TestCorrelationPerfectPositive tests perfect positive correlation
func TestCorrelationPerfectPositive(t *testing.T) {
	x := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	y := []float64{2.0, 4.0, 6.0, 8.0, 10.0}

	corr, err := Correlation(x, y)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	if !floatEquals(corr, 1.0, testEpsilon) {
		t.Errorf("Expected correlation 1.0, got %f", corr)
	}
}

// TestCorrelationPerfectNegative tests perfect negative correlation
func TestCorrelationPerfectNegative(t *testing.T) {
	x := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	y := []float64{10.0, 8.0, 6.0, 4.0, 2.0}

	corr, err := Correlation(x, y)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	if !floatEquals(corr, -1.0, testEpsilon) {
		t.Errorf("Expected correlation -1.0, got %f", corr)
	}
}

// TestCorrelationZero tests zero correlation (constant variable)
func TestCorrelationZero(t *testing.T) {
	x := []float64{1.0, 2.0, 3.0, 4.0, 5.0}
	y := []float64{3.0, 3.0, 3.0, 3.0, 3.0}

	corr, err := Correlation(x, y)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	if !floatEquals(corr, 0.0, testEpsilon) {
		t.Errorf("Expected correlation 0.0, got %f", corr)
	}
}

// TestCorrelationKnownValue tests correlation with known value
func TestCorrelationKnownValue(t *testing.T) {
	x := []float64{1.0, 2.0, 3.0, 4.0, 5.0, 6.0}
	y := []float64{2.5, 3.5, 4.0, 5.5, 6.0, 7.5}

	corr, err := Correlation(x, y)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	expected := 0.990478003842680
	if !floatEquals(corr, expected, 1e-6) {
		t.Errorf("Expected correlation %f, got %f", expected, corr)
	}
}

// TestCorrelationInvalidInput tests error handling for invalid inputs
func TestCorrelationInvalidInput(t *testing.T) {
	tests := []struct {
		name string
		x    []float64
		y    []float64
	}{
		{"empty slices", []float64{}, []float64{}},
		{"single element", []float64{1.0}, []float64{2.0}},
		{"different lengths", []float64{1.0, 2.0, 3.0}, []float64{1.0, 2.0}},
		{"nil slices", nil, nil},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, err := Correlation(tt.x, tt.y)
			if err == nil {
				t.Error("Expected error for invalid input, got nil")
			}
		})
	}
}

// TestCorrelationNaN tests NaN handling
func TestCorrelationNaN(t *testing.T) {
	x := []float64{1.0, 2.0, math.NaN(), 4.0}
	y := []float64{2.0, 4.0, 6.0, 8.0}

	_, err := Correlation(x, y)
	if err != ErrNaNInput {
		t.Errorf("Expected ErrNaNInput, got %v", err)
	}
}

// TestCorrelationMatrixIdentity tests 2x2 identity case
func TestCorrelationMatrixIdentity(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0, 4.0}, // variable 0
		{1.0, 2.0, 3.0, 4.0}, // variable 1 (identical to var 0)
	}

	corrMatrix, err := CorrelationMatrix(data)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// Check dimensions
	if len(corrMatrix) != 2 || len(corrMatrix[0]) != 2 {
		t.Fatalf("Expected 2x2 matrix, got %dx%d", len(corrMatrix), len(corrMatrix[0]))
	}

	// Check diagonal
	if !floatEquals(corrMatrix[0][0], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][0] = 1.0, got %f", corrMatrix[0][0])
	}
	if !floatEquals(corrMatrix[1][1], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[1][1] = 1.0, got %f", corrMatrix[1][1])
	}

	// Check off-diagonal (perfect correlation)
	if !floatEquals(corrMatrix[0][1], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][1] = 1.0, got %f", corrMatrix[0][1])
	}
	if !floatEquals(corrMatrix[1][0], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[1][0] = 1.0, got %f", corrMatrix[1][0])
	}
}

// TestCorrelationMatrix3x3 tests 3x3 matrix with known correlations
func TestCorrelationMatrix3x3(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0, 4.0, 5.0}, // variable 0
		{2.0, 4.0, 6.0, 8.0, 10.0}, // variable 1 (2 * var0)
		{5.0, 4.0, 3.0, 2.0, 1.0}, // variable 2 (inverse of var0)
	}

	corrMatrix, err := CorrelationMatrix(data)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// Check dimensions
	if len(corrMatrix) != 3 || len(corrMatrix[0]) != 3 {
		t.Fatalf("Expected 3x3 matrix, got %dx%d", len(corrMatrix), len(corrMatrix[0]))
	}

	// Check diagonal
	for i := 0; i < 3; i++ {
		if !floatEquals(corrMatrix[i][i], 1.0, testEpsilon) {
			t.Errorf("Expected corrMatrix[%d][%d] = 1.0, got %f", i, i, corrMatrix[i][i])
		}
	}

	// Var0 and Var1: perfect positive correlation
	if !floatEquals(corrMatrix[0][1], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][1] = 1.0, got %f", corrMatrix[0][1])
	}

	// Var0 and Var2: perfect negative correlation
	if !floatEquals(corrMatrix[0][2], -1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][2] = -1.0, got %f", corrMatrix[0][2])
	}

	// Var1 and Var2: perfect negative correlation
	if !floatEquals(corrMatrix[1][2], -1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[1][2] = -1.0, got %f", corrMatrix[1][2])
	}
}

// TestCorrelationMatrixSymmetry tests matrix symmetry
func TestCorrelationMatrixSymmetry(t *testing.T) {
	data := [][]float64{
		{1.5, 2.1, 3.2, 4.0},
		{2.3, 3.5, 4.1, 5.2},
		{3.1, 2.8, 3.5, 4.2},
	}

	corrMatrix, err := CorrelationMatrix(data)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// Check symmetry
	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			if !floatEquals(corrMatrix[i][j], corrMatrix[j][i], testEpsilon) {
				t.Errorf("Matrix not symmetric: corrMatrix[%d][%d] = %f, corrMatrix[%d][%d] = %f",
					i, j, corrMatrix[i][j], j, i, corrMatrix[j][i])
			}
		}
	}
}

// TestCorrelationMatrixZeroVariance tests handling of zero variance
func TestCorrelationMatrixZeroVariance(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0, 4.0}, // variable 0 (has variance)
		{5.0, 5.0, 5.0, 5.0}, // variable 1 (zero variance)
	}

	corrMatrix, err := CorrelationMatrix(data)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// Diagonal should be 1.0
	if !floatEquals(corrMatrix[0][0], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][0] = 1.0, got %f", corrMatrix[0][0])
	}
	if !floatEquals(corrMatrix[1][1], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[1][1] = 1.0, got %f", corrMatrix[1][1])
	}

	// Off-diagonal should be 0.0 (zero variance)
	if !floatEquals(corrMatrix[0][1], 0.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][1] = 0.0, got %f", corrMatrix[0][1])
	}
	if !floatEquals(corrMatrix[1][0], 0.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[1][0] = 0.0, got %f", corrMatrix[1][0])
	}
}

// TestCorrelationMatrixFlat tests flat matrix output
func TestCorrelationMatrixFlat(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0},
		{2.0, 4.0, 6.0},
	}

	corrMatrix, nVars, err := CorrelationMatrixFlat(data)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	if nVars != 2 {
		t.Errorf("Expected nVars = 2, got %d", nVars)
	}

	if len(corrMatrix) != 4 {
		t.Fatalf("Expected flat matrix length 4, got %d", len(corrMatrix))
	}

	// Check values (row-major order)
	if !floatEquals(corrMatrix[0], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0] = 1.0, got %f", corrMatrix[0])
	}
	if !floatEquals(corrMatrix[1], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[1] = 1.0, got %f", corrMatrix[1])
	}
	if !floatEquals(corrMatrix[2], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[2] = 1.0, got %f", corrMatrix[2])
	}
	if !floatEquals(corrMatrix[3], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[3] = 1.0, got %f", corrMatrix[3])
	}
}

// TestCorrelationMatrixPrecomputed tests precomputed statistics version
func TestCorrelationMatrixPrecomputed(t *testing.T) {
	data := [][]float64{
		{1.0, 2.0, 3.0, 4.0},
		{2.0, 4.0, 6.0, 8.0},
	}

	// Compute means and stddevs
	means := []float64{2.5, 5.0}
	stddevs := []float64{1.29099444873581, 2.58198889747161}

	corrMatrix, err := CorrelationMatrixPrecomputed(data, means, stddevs)
	if err != nil {
		t.Fatalf("Unexpected error: %v", err)
	}

	// Check diagonal
	if !floatEquals(corrMatrix[0][0], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][0] = 1.0, got %f", corrMatrix[0][0])
	}
	if !floatEquals(corrMatrix[1][1], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[1][1] = 1.0, got %f", corrMatrix[1][1])
	}

	// Check off-diagonal (perfect correlation)
	if !floatEquals(corrMatrix[0][1], 1.0, testEpsilon) {
		t.Errorf("Expected corrMatrix[0][1] = 1.0, got %f", corrMatrix[0][1])
	}
}

// TestCorrelationMatrixInvalidInput tests error handling
func TestCorrelationMatrixInvalidInput(t *testing.T) {
	tests := []struct {
		name string
		data [][]float64
	}{
		{"empty data", [][]float64{}},
		{"single sample", [][]float64{{1.0}, {2.0}}},
		{"inconsistent lengths", [][]float64{{1.0, 2.0}, {1.0, 2.0, 3.0}}},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			_, err := CorrelationMatrix(tt.data)
			if err == nil {
				t.Error("Expected error for invalid input, got nil")
			}
		})
	}
}

// BenchmarkCorrelation benchmarks pairwise correlation
func BenchmarkCorrelation(b *testing.B) {
	sizes := []int{100, 250, 1000, 10000}

	for _, n := range sizes {
		x := make([]float64, n)
		y := make([]float64, n)
		for i := 0; i < n; i++ {
			x[i] = float64(i) * 0.5
			y[i] = float64(i) * 0.3 + 10.0
		}

		b.Run(string(rune(n)), func(b *testing.B) {
			b.ResetTimer()
			for i := 0; i < b.N; i++ {
				_, _ = Correlation(x, y)
			}
		})
	}
}

// BenchmarkCorrelationMatrix benchmarks correlation matrix computation
func BenchmarkCorrelationMatrix(b *testing.B) {
	configs := []struct {
		nVars    int
		nSamples int
	}{
		{10, 250},
		{50, 250},
		{100, 250},
		{500, 250},
	}

	for _, cfg := range configs {
		data := make([][]float64, cfg.nVars)
		for i := 0; i < cfg.nVars; i++ {
			data[i] = make([]float64, cfg.nSamples)
			for j := 0; j < cfg.nSamples; j++ {
				data[i][j] = float64(i+j) * 0.5
			}
		}

		b.Run(string(rune(cfg.nVars))+"vars", func(b *testing.B) {
			b.ResetTimer()
			for i := 0; i < b.N; i++ {
				_, _ = CorrelationMatrix(data)
			}
		})
	}
}

// BenchmarkCorrelationMatrixFlat benchmarks flat matrix version
func BenchmarkCorrelationMatrixFlat(b *testing.B) {
	const nVars = 100
	const nSamples = 250

	data := make([][]float64, nVars)
	for i := 0; i < nVars; i++ {
		data[i] = make([]float64, nSamples)
		for j := 0; j < nSamples; j++ {
			data[i][j] = float64(i+j) * 0.5
		}
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _, _ = CorrelationMatrixFlat(data)
	}
}
