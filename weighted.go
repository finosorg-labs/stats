package stats

/*
#include "stats.h"
#include "weighted.h"
*/
import "C"
import "unsafe"

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
	if len(data) != len(weights) || len(data) != nGroups*groupSize {
		return ErrInvalidArg
	}
	return nil
}
