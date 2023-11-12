// statistical_features.cpp
#include "statistical_features.h"
#include <cmath>

// Define the function implementation
StatisticalFeatures calculateStatisticalFeatures(const std::vector<double>& signalData) {
    StatisticalFeatures statisticalFeatures;
    // Calculate mean
    double sum = 0.0;
    for (const double& value : signalData) {
        sum += value;
    }
    double mean = sum / signalData.size();
    // Calculate variance
    double variance = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        variance += deviation * deviation;
    }
    variance /= signalData.size();
    // Calculate skewness
    double skewness = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        skewness += (deviation * deviation * deviation);
    }
    skewness /= (signalData.size() * std::pow(variance, 1.5));
    // Calculate kurtosis
    double kurtosis = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        kurtosis += (deviation * deviation * deviation * deviation);
    }
    kurtosis /= (signalData.size() * variance * variance);
    statisticalFeatures.mean = mean;
    statisticalFeatures.variance = variance;
    statisticalFeatures.skewness = skewness;
    statisticalFeatures.kurtosis = kurtosis;

    return statisticalFeatures;
}