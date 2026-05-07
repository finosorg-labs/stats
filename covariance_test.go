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
