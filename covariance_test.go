package stats

import (
	"math"
	"testing"
)

const epsilon = 1e-10

func TestCovarianceBasic(t *testing.T) {
	// Simple 2x2 case with known result
	// Data: [[1, 2], [3, 4], [5, 6]]
	// Mean: [3, 4]
	// Centered: [[-2, -2], [0, 0], [2, 2]]
	// Cov = [[-2, 0, 2]^T * [-2, 0, 2]] / 2 = [[8, 8], [8, 8]] / 2 = [[4, 4], [4, 4]]
	data := []float64{1, 2, 3, 4, 5, 6}

	cov, err := Covariance(data, 3, 2, true)
	if err != nil {
		t.Fatalf("Covariance failed: %v", err)
	}

	expected := []float64{4.0, 4.0, 4.0, 4.0}
	for i := 0; i < 4; i++ {
		if math.Abs(cov[i]-expected[i]) > epsilon {
			t.Errorf("Cov[%d] = %f, expected %f", i, cov[i], expected[i])
		}
	}
}

func TestCovariancePopulation(t *testing.T) {
	// Test population covariance (divide by n instead of n-1)
	data := []float64{1, 2, 3, 4, 5, 6}

	cov, err := Covariance(data, 3, 2, false)
	if err != nil {
		t.Fatalf("Population covariance failed: %v", err)
	}

	// Population covariance should be 2/3 of sample covariance
	expected := 8.0 / 3.0
	if math.Abs(cov[0]-expected) > epsilon {
		t.Errorf("Population Cov[0,0] = %f, expected %f", cov[0], expected)
	}
}

func TestCovarianceSymmetric(t *testing.T) {
	// Covariance matrix should be symmetric
	data := make([]float64, 30)
	for i := 0; i < 30; i++ {
		data[i] = float64(i%7) + 0.5*float64(i%3)
	}

	cov, err := Covariance(data, 10, 3, true)
	if err != nil {
		t.Fatalf("Covariance failed: %v", err)
	}

	// Check symmetry
	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			if math.Abs(cov[i*3+j]-cov[j*3+i]) > epsilon {
				t.Errorf("Matrix not symmetric at [%d,%d]: %f != %f",
					i, j, cov[i*3+j], cov[j*3+i])
			}
		}
	}
}

func TestCorrelationBasic(t *testing.T) {
	// Perfect positive correlation
	cov := []float64{4.0, 4.0, 4.0, 4.0}

	corr, err := Correlation(cov, 2)
	if err != nil {
		t.Fatalf("Correlation failed: %v", err)
	}

	// All elements should be 1.0 for perfect correlation
	for i := 0; i < 4; i++ {
		if math.Abs(corr[i]-1.0) > epsilon {
			t.Errorf("Corr[%d] = %f, expected 1.0", i, corr[i])
		}
	}
}

func TestCorrelationUncorrelated(t *testing.T) {
	// Zero covariance (identity covariance matrix)
	cov := []float64{1.0, 0.0, 0.0, 1.0}

	corr, err := Correlation(cov, 2)
	if err != nil {
		t.Fatalf("Correlation failed: %v", err)
	}

	// Diagonal should be 1.0, off-diagonal should be 0.0
	if math.Abs(corr[0]-1.0) > epsilon {
		t.Errorf("Corr[0,0] = %f, expected 1.0", corr[0])
	}
	if math.Abs(corr[1]-0.0) > epsilon {
		t.Errorf("Corr[0,1] = %f, expected 0.0", corr[1])
	}
	if math.Abs(corr[2]-0.0) > epsilon {
		t.Errorf("Corr[1,0] = %f, expected 0.0", corr[2])
	}
	if math.Abs(corr[3]-1.0) > epsilon {
		t.Errorf("Corr[1,1] = %f, expected 1.0", corr[3])
	}
}

func TestCorrelationRange(t *testing.T) {
	// Correlation coefficients should be in [-1, 1]
	data := make([]float64, 100)
	for i := 0; i < 100; i++ {
		data[i] = math.Sin(float64(i)*0.1) + math.Cos(float64(i)*0.2)
	}

	cov, corr, err := CovarianceCorrelation(data, 20, 5, true)
	if err != nil {
		t.Fatalf("CovarianceCorrelation failed: %v", err)
	}

	if cov == nil {
		t.Fatal("Covariance matrix is nil")
	}

	// Check correlation range
	for i := 0; i < 5; i++ {
		for j := 0; j < 5; j++ {
			if corr[i*5+j] < -1.0-epsilon || corr[i*5+j] > 1.0+epsilon {
				t.Errorf("Corr[%d,%d] = %f out of range [-1, 1]", i, j, corr[i*5+j])
			}
		}
	}

	// Check diagonal is 1.0
	for i := 0; i < 5; i++ {
		if math.Abs(corr[i*5+i]-1.0) > epsilon {
			t.Errorf("Corr[%d,%d] = %f, expected 1.0", i, i, corr[i*5+i])
		}
	}
}

func TestCovarianceInvalidInput(t *testing.T) {
	data := []float64{1, 2, 3, 4, 5, 6}

	// Zero samples
	_, err := Covariance(data, 0, 2, true)
	if err == nil {
		t.Error("Should reject zero samples")
	}

	// Zero variables
	_, err = Covariance(data, 3, 0, true)
	if err == nil {
		t.Error("Should reject zero variables")
	}

	// Insufficient samples for sample covariance
	_, err = Covariance([]float64{1, 2}, 1, 2, true)
	if err == nil {
		t.Error("Should reject n=1 for sample covariance")
	}

	// Population covariance should work with n=1
	_, err = Covariance([]float64{1, 2}, 1, 2, false)
	if err != nil {
		t.Errorf("Population covariance should work with n=1: %v", err)
	}
}

func TestCovarianceLargeMatrix(t *testing.T) {
	// Test with larger matrix (50 samples, 10 variables)
	const nSamples = 50
	const nVars = 10

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i)*0.1) + math.Cos(float64(i)*0.05)
	}

	cov, err := Covariance(data, nSamples, nVars, true)
	if err != nil {
		t.Fatalf("Large matrix covariance failed: %v", err)
	}

	// Verify symmetry
	for i := 0; i < nVars; i++ {
		for j := 0; j < nVars; j++ {
			if math.Abs(cov[i*nVars+j]-cov[j*nVars+i]) > epsilon {
				t.Errorf("Large matrix not symmetric at [%d,%d]", i, j)
			}
		}
	}

	// Verify diagonal is positive
	for i := 0; i < nVars; i++ {
		if cov[i*nVars+i] < 0.0 {
			t.Errorf("Variance[%d] = %f should be non-negative", i, cov[i*nVars+i])
		}
	}
}

func TestCovarianceConstantVariable(t *testing.T) {
	// One variable is constant (zero variance)
	data := []float64{1, 5, 2, 5, 3, 5, 4, 5}

	cov, err := Covariance(data, 4, 2, true)
	if err != nil {
		t.Fatalf("Should handle constant variable: %v", err)
	}

	// Second variable has zero variance
	if math.Abs(cov[3]) > epsilon {
		t.Errorf("Constant variable should have zero variance, got %f", cov[3])
	}
}

func BenchmarkCovarianceSmall(b *testing.B) {
	const nSamples = 100
	const nVars = 5

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i) * 0.1)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Covariance(data, nSamples, nVars, true)
	}
}

func BenchmarkCovarianceMedium(b *testing.B) {
	const nSamples = 1000
	const nVars = 20

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i) * 0.1)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Covariance(data, nSamples, nVars, true)
	}
}

func BenchmarkCovarianceLarge(b *testing.B) {
	const nSamples = 5000
	const nVars = 50

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i) * 0.1)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Covariance(data, nSamples, nVars, true)
	}
}

func BenchmarkCovarianceFinancial(b *testing.B) {
	// Typical financial scenario: 250 trading days, 500 stocks
	const nSamples = 250
	const nVars = 500

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i)*0.1) + math.Cos(float64(i)*0.05)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Covariance(data, nSamples, nVars, true)
	}
}

func BenchmarkCorrelationSmall(b *testing.B) {
	const nVars = 10

	cov := make([]float64, nVars*nVars)
	for i := 0; i < nVars; i++ {
		for j := 0; j < nVars; j++ {
			if i == j {
				cov[i*nVars+j] = 1.0
			} else {
				cov[i*nVars+j] = 0.5 / (1.0 + math.Abs(float64(i-j)))
			}
		}
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Correlation(cov, nVars)
	}
}

func BenchmarkCovarianceCorrelationCombined(b *testing.B) {
	const nSamples = 250
	const nVars = 100

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i)*0.1) + math.Cos(float64(i)*0.05)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _, _ = CovarianceCorrelation(data, nSamples, nVars, true)
	}
}

func TestSpearmanPerfectMonotonic(t *testing.T) {
	// Perfect monotonic relationship: y = 2*x + 1
	data := []float64{1, 3, 2, 5, 3, 7, 4, 9, 5, 11}

	corr, err := Spearman(data, 5, 2)
	if err != nil {
		t.Fatalf("Spearman failed: %v", err)
	}

	// Perfect monotonic relationship should give correlation = 1.0
	if math.Abs(corr[0]-1.0) > epsilon {
		t.Errorf("Corr[0,0] = %f, expected 1.0", corr[0])
	}
	if math.Abs(corr[1]-1.0) > epsilon {
		t.Errorf("Corr[0,1] = %f, expected 1.0", corr[1])
	}
	if math.Abs(corr[2]-1.0) > epsilon {
		t.Errorf("Corr[1,0] = %f, expected 1.0", corr[2])
	}
	if math.Abs(corr[3]-1.0) > epsilon {
		t.Errorf("Corr[1,1] = %f, expected 1.0", corr[3])
	}
}

func TestSpearmanPerfectNegative(t *testing.T) {
	// Perfect negative monotonic relationship
	data := []float64{1, 10, 2, 8, 3, 6, 4, 4, 5, 2}

	corr, err := Spearman(data, 5, 2)
	if err != nil {
		t.Fatalf("Spearman failed: %v", err)
	}

	// Perfect negative monotonic relationship should give correlation = -1.0
	if math.Abs(corr[0]-1.0) > epsilon {
		t.Errorf("Corr[0,0] = %f, expected 1.0", corr[0])
	}
	if math.Abs(corr[1]-(-1.0)) > epsilon {
		t.Errorf("Corr[0,1] = %f, expected -1.0", corr[1])
	}
	if math.Abs(corr[2]-(-1.0)) > epsilon {
		t.Errorf("Corr[1,0] = %f, expected -1.0", corr[2])
	}
	if math.Abs(corr[3]-1.0) > epsilon {
		t.Errorf("Corr[1,1] = %f, expected 1.0", corr[3])
	}
}

func TestSpearmanUncorrelated(t *testing.T) {
	// Uncorrelated variables
	data := []float64{1, 5, 2, 3, 3, 1, 4, 4, 5, 2}

	corr, err := Spearman(data, 5, 2)
	if err != nil {
		t.Fatalf("Spearman failed: %v", err)
	}

	// Diagonal should be 1.0
	if math.Abs(corr[0]-1.0) > epsilon {
		t.Errorf("Corr[0,0] = %f, expected 1.0", corr[0])
	}
	if math.Abs(corr[3]-1.0) > epsilon {
		t.Errorf("Corr[1,1] = %f, expected 1.0", corr[3])
	}

	// Off-diagonal should be in valid range
	if corr[1] < -1.0 || corr[1] > 1.0 {
		t.Errorf("Corr[0,1] = %f out of range [-1, 1]", corr[1])
	}
	if corr[2] < -1.0 || corr[2] > 1.0 {
		t.Errorf("Corr[1,0] = %f out of range [-1, 1]", corr[2])
	}
}

func TestSpearmanWithTies(t *testing.T) {
	// Data with tied values
	data := []float64{1, 2, 2, 3, 2, 3, 3, 4, 4, 5}

	corr, err := Spearman(data, 5, 2)
	if err != nil {
		t.Fatalf("Spearman failed: %v", err)
	}

	// Diagonal should be 1.0
	if math.Abs(corr[0]-1.0) > epsilon {
		t.Errorf("Corr[0,0] = %f, expected 1.0", corr[0])
	}
	if math.Abs(corr[3]-1.0) > epsilon {
		t.Errorf("Corr[1,1] = %f, expected 1.0", corr[3])
	}

	// Correlation should be in valid range (with tolerance for floating-point errors)
	if corr[1] < -1.0-epsilon || corr[1] > 1.0+epsilon {
		t.Errorf("Corr[0,1] = %.17f out of range [-1, 1]", corr[1])
	}

	// Matrix should be symmetric
	if math.Abs(corr[1]-corr[2]) > epsilon {
		t.Errorf("Matrix not symmetric: Corr[0,1]=%f != Corr[1,0]=%f", corr[1], corr[2])
	}
}

func TestSpearmanNonlinear(t *testing.T) {
	// Nonlinear but monotonic relationship: y = x^2
	data := []float64{1, 1, 2, 4, 3, 9, 4, 16, 5, 25}

	corr, err := Spearman(data, 5, 2)
	if err != nil {
		t.Fatalf("Spearman failed: %v", err)
	}

	// Spearman should detect monotonic relationship even if nonlinear
	if math.Abs(corr[1]-1.0) > epsilon {
		t.Errorf("Corr[0,1] = %f, expected 1.0 for monotonic relationship", corr[1])
	}
	if math.Abs(corr[2]-1.0) > epsilon {
		t.Errorf("Corr[1,0] = %f, expected 1.0 for monotonic relationship", corr[2])
	}
}

func TestSpearmanInvalidInput(t *testing.T) {
	data := []float64{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}

	// Zero samples
	_, err := Spearman(data, 0, 2)
	if err == nil {
		t.Error("Should reject zero samples")
	}

	// Zero variables
	_, err = Spearman(data, 5, 0)
	if err == nil {
		t.Error("Should reject zero variables")
	}

	// Insufficient samples
	_, err = Spearman([]float64{1, 2}, 1, 2)
	if err == nil {
		t.Error("Should reject n=1 for Spearman")
	}

	// Length mismatch
	_, err = Spearman(data, 3, 2)
	if err == nil {
		t.Error("Should reject length mismatch")
	}
}

func TestSpearmanSymmetric(t *testing.T) {
	// Spearman correlation matrix should be symmetric
	data := make([]float64, 30)
	for i := 0; i < 30; i++ {
		data[i] = float64(i%7) + 0.5*float64(i%3)
	}

	corr, err := Spearman(data, 10, 3)
	if err != nil {
		t.Fatalf("Spearman failed: %v", err)
	}

	// Check symmetry
	for i := 0; i < 3; i++ {
		for j := 0; j < 3; j++ {
			if math.Abs(corr[i*3+j]-corr[j*3+i]) > epsilon {
				t.Errorf("Matrix not symmetric at [%d,%d]: %f != %f",
					i, j, corr[i*3+j], corr[j*3+i])
			}
		}
	}
}

func TestSpearmanLargeMatrix(t *testing.T) {
	// Test with larger matrix
	const nSamples = 50
	const nVars = 10

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i)*0.1) + math.Cos(float64(i)*0.05)
	}

	corr, err := Spearman(data, nSamples, nVars)
	if err != nil {
		t.Fatalf("Large matrix Spearman failed: %v", err)
	}

	// Verify symmetry
	for i := 0; i < nVars; i++ {
		for j := 0; j < nVars; j++ {
			if math.Abs(corr[i*nVars+j]-corr[j*nVars+i]) > epsilon {
				t.Errorf("Large matrix not symmetric at [%d,%d]", i, j)
			}
		}
	}

	// Verify diagonal is 1.0
	for i := 0; i < nVars; i++ {
		if math.Abs(corr[i*nVars+i]-1.0) > epsilon {
			t.Errorf("Diagonal[%d] = %f, expected 1.0", i, corr[i*nVars+i])
		}
	}

	// Verify range [-1, 1]
	for i := 0; i < nVars*nVars; i++ {
		if corr[i] < -1.0-epsilon || corr[i] > 1.0+epsilon {
			t.Errorf("Corr[%d] = %f out of range [-1, 1]", i, corr[i])
		}
	}
}

func BenchmarkSpearmanSmall(b *testing.B) {
	const nSamples = 100
	const nVars = 5

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i) * 0.1)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Spearman(data, nSamples, nVars)
	}
}

func BenchmarkSpearmanMedium(b *testing.B) {
	const nSamples = 1000
	const nVars = 20

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i) * 0.1)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Spearman(data, nSamples, nVars)
	}
}

func BenchmarkSpearmanLarge(b *testing.B) {
	const nSamples = 5000
	const nVars = 50

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i) * 0.1)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Spearman(data, nSamples, nVars)
	}
}

func BenchmarkSpearmanFinancial(b *testing.B) {
	// Typical financial scenario: 250 trading days, 100 stocks
	const nSamples = 250
	const nVars = 100

	data := make([]float64, nSamples*nVars)
	for i := 0; i < nSamples*nVars; i++ {
		data[i] = math.Sin(float64(i)*0.1) + math.Cos(float64(i)*0.05)
	}

	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		_, _ = Spearman(data, nSamples, nVars)
	}
}
