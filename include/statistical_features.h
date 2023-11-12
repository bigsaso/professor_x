// statistical_features.h
#ifndef STATISTICALFEATURES_H
#define STATISTICALFEATURES_H

#include <vector>

// Define the struct for statistical features
struct StatisticalFeatures {
    double mean;
    double variance;
    double skewness;
    double kurtosis;
};

// Declare the function prototype
StatisticalFeatures calculateStatisticalFeatures(const std::vector<double>& signalData);

#endif