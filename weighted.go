package stats

/*
#include "stats.h"
#include "weighted.h"
*/
import "C"
import "unsafe"

// WeightedSum computes the weighted sum of a float64 slice.
func WeightedSum(data, weights []float64) (float64, error) {
	if len(data) == 0 || len(data) != len(weights) {
		return 0, ErrInvalidArg
	}

	var sum C.double
	status := C.fc_stats_weighted_sum_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(len(data)),
		&sum,
	)
	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(sum), nil
}

// WeightedMean computes the weighted mean of a float64 slice.
func WeightedMean(data, weights []float64) (float64, error) {
	if len(data) == 0 || len(data) != len(weights) {
		return 0, ErrInvalidArg
	}

	var mean C.double
	status := C.fc_stats_weighted_mean_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(len(data)),
		&mean,
	)
	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(mean), nil
}

// WeightedVariance computes the weighted population variance of a float64 slice.
func WeightedVariance(data, weights []float64) (float64, error) {
	if len(data) == 0 || len(data) != len(weights) {
		return 0, ErrInvalidArg
	}

	var variance C.double
	status := C.fc_stats_weighted_variance_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(len(data)),
		&variance,
	)
	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(variance), nil
}

// WeightedMeanVariance computes the weighted mean and population variance simultaneously.
func WeightedMeanVariance(data, weights []float64) (mean, variance float64, err error) {
	if len(data) == 0 || len(data) != len(weights) {
		return 0, 0, ErrInvalidArg
	}

	var cMean, cVariance C.double
	status := C.fc_stats_weighted_mean_variance_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(len(data)),
		&cMean,
		&cVariance,
	)
	if err := statusToError(status); err != nil {
		return 0, 0, err
	}

	return float64(cMean), float64(cVariance), nil
}

// WeightedStdDev computes the weighted population standard deviation of a float64 slice.
func WeightedStdDev(data, weights []float64) (float64, error) {
	if len(data) == 0 || len(data) != len(weights) {
		return 0, ErrInvalidArg
	}

	var stddev C.double
	status := C.fc_stats_weighted_stddev_f64(
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(len(data)),
		&stddev,
	)
	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(stddev), nil
}

// WeightedSumBatch computes weighted sums for flat groups.
func WeightedSumBatch(data, weights []float64, nGroups, groupSize int) ([]float64, error) {
	if err := validateWeightedBatch(data, weights, nGroups, groupSize); err != nil {
		return nil, err
	}

	sums := make([]float64, nGroups)
	status := C.fc_stats_weighted_sum_batch_f64(
		(*C.double)(unsafe.Pointer(&sums[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)
	if err := statusToError(status); err != nil {
		return nil, err
	}

	return sums, nil
}

// WeightedMeanBatch computes weighted means for flat groups.
func WeightedMeanBatch(data, weights []float64, nGroups, groupSize int) ([]float64, error) {
	if err := validateWeightedBatch(data, weights, nGroups, groupSize); err != nil {
		return nil, err
	}

	means := make([]float64, nGroups)
	status := C.fc_stats_weighted_mean_batch_f64(
		(*C.double)(unsafe.Pointer(&means[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)
	if err := statusToError(status); err != nil {
		return nil, err
	}

	return means, nil
}

// WeightedVarianceBatch computes weighted population variances for flat groups.
func WeightedVarianceBatch(data, weights []float64, nGroups, groupSize int) ([]float64, error) {
	if err := validateWeightedBatch(data, weights, nGroups, groupSize); err != nil {
		return nil, err
	}

	variances := make([]float64, nGroups)
	status := C.fc_stats_weighted_variance_batch_f64(
		(*C.double)(unsafe.Pointer(&variances[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)
	if err := statusToError(status); err != nil {
		return nil, err
	}

	return variances, nil
}

// WeightedMeanVarianceBatch computes weighted means and population variances for flat groups.
func WeightedMeanVarianceBatch(data, weights []float64, nGroups, groupSize int) ([]float64, []float64, error) {
	if err := validateWeightedBatch(data, weights, nGroups, groupSize); err != nil {
		return nil, nil, err
	}

	means := make([]float64, nGroups)
	variances := make([]float64, nGroups)
	status := C.fc_stats_weighted_mean_variance_batch_f64(
		(*C.double)(unsafe.Pointer(&means[0])),
		(*C.double)(unsafe.Pointer(&variances[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)
	if err := statusToError(status); err != nil {
		return nil, nil, err
	}

	return means, variances, nil
}

// WeightedStdDevBatch computes weighted population standard deviations for flat groups.
func WeightedStdDevBatch(data, weights []float64, nGroups, groupSize int) ([]float64, error) {
	if err := validateWeightedBatch(data, weights, nGroups, groupSize); err != nil {
		return nil, err
	}

	stddevs := make([]float64, nGroups)
	status := C.fc_stats_weighted_stddev_batch_f64(
		(*C.double)(unsafe.Pointer(&stddevs[0])),
		(*C.double)(unsafe.Pointer(&data[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(nGroups),
		C.size_t(groupSize),
	)
	if err := statusToError(status); err != nil {
		return nil, err
	}

	return stddevs, nil
}

func validateWeightedBatch(data, weights []float64, nGroups, groupSize int) error {
	if nGroups <= 0 || groupSize <= 0 {
		return ErrInvalidArg
	}
	if len(data) != len(weights) || nGroups > len(data)/groupSize || len(data) != nGroups*groupSize {
		return ErrInvalidArg
	}
	return nil
}

// WeightedMedian computes the weighted median for a single dataset.
//
// The weighted median is the value at which cumulative weight reaches 50% of total weight.
// Unlike weighted mean, it is robust against outliers and manipulation, making it ideal
// for aggregating prices from multiple exchanges.
//
// Example:
//
//	prices := []float64{50000, 50050, 49900, 50020, 51000}
//	volumes := []float64{100, 150, 120, 130, 10}
//	median, err := WeightedMedian(prices, volumes)
func WeightedMedian(values, weights []float64) (float64, error) {
	if len(values) == 0 || len(values) != len(weights) {
		return 0, ErrInvalidArg
	}

	var result C.double
	status := C.fc_stats_weighted_median_f64(
		(*C.double)(unsafe.Pointer(&values[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		C.size_t(len(values)),
		&result,
	)
	if err := statusToError(status); err != nil {
		return 0, err
	}

	return float64(result), nil
}

// WeightedMedianBatch computes weighted medians for multiple datasets in batch.
//
// Each dataset can have different size. The function processes all datasets efficiently.
// Commonly used for computing mark prices across multiple trading contracts.
//
// Example:
//
//	// Dataset 0: 3 exchanges
//	// Dataset 1: 5 exchanges
//	values := []float64{100, 200, 300, 10, 20, 30, 40, 50}
//	weights := []float64{1, 1, 1, 2, 2, 2, 2, 2}
//	offsets := []int{0, 3}
//	sizes := []int{3, 5}
//	results, err := WeightedMedianBatch(values, weights, offsets, sizes)
func WeightedMedianBatch(values, weights []float64, offsets, sizes []int) ([]float64, error) {
	if len(offsets) == 0 || len(sizes) == 0 || len(offsets) != len(sizes) {
		return nil, ErrInvalidArg
	}

	batchSize := len(offsets)
	results := make([]float64, batchSize)

	cOffsets := make([]C.size_t, batchSize)
	cSizes := make([]C.size_t, batchSize)
	for i := 0; i < batchSize; i++ {
		cOffsets[i] = C.size_t(offsets[i])
		cSizes[i] = C.size_t(sizes[i])
	}

	status := C.fc_stats_weighted_median_batch_f64(
		(*C.double)(unsafe.Pointer(&values[0])),
		(*C.double)(unsafe.Pointer(&weights[0])),
		(*C.size_t)(unsafe.Pointer(&cOffsets[0])),
		(*C.size_t)(unsafe.Pointer(&cSizes[0])),
		C.size_t(batchSize),
		(*C.double)(unsafe.Pointer(&results[0])),
	)
	if err := statusToError(status); err != nil {
		return nil, err
	}

	return results, nil
}
