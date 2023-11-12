// features_analysis.h
#ifndef FEATURES_ANALYSIS_H
#define FEATURES_ANALYSIS_H

#include <vector>
#include "statistical_features.h"
#include "fftw.h"
#include "fftw_relative_powers.h"
#include "shannon_entropy.h"
#include "zero_crossing_rate.h"
#include "hjorth_parameters.h"
#include "stft_relative_powers.h"

// Struct for each buffer analysis features
struct Features {
    int numSamples;
    StatisticalFeatures statisticalFeatures;
    RelativePowers fftwRelativePowers;
    double entropy;
    int zeroCrossingRate;
    HjorthParameters hjorthParams;
    std::vector<RelativePowers> stftRelativePowers;
};

// Declaration of analyze function
Features analyze(const std::vector<double> buffer);

#endif