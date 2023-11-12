// features_analysis.cpp
#include "features_analysis.h"

// Analyze function
Features analyze(const std::vector<double> buffer) {

    // Calculate size of the signal
    int N = buffer.size();

    // Define frequency bands (for relative power)
    double deltaMin = 0.5; // Lower bound of delta band (Hz)
    double deltaMax = 4.0; // Upper bound of delta band (Hz)
    double thetaMin = 4.0; // Lower bound of theta band (Hz)
    double thetaMax = 8.0; // Upper bound of theta band (Hz)
    double alphaMin = 8.0; // Lower bound of alpha band (Hz)
    double alphaMax = 13.0; // Upper bound of alpha band (Hz)
    double betaMin = 13.0; // Lower bound of beta band (Hz)
    double betaMax = 30.0; // Upper bound of beta band (Hz)
    double gammaMin = 30.0; // Lower bound of gamma band (Hz)
    double gammaMax = 160.0; // Upper bound of gamma band (Hz). This one will change based on headset's frequency.
    // Mu rhythm is associated with the sensorimotor cortex and can be attenuated during motor tasks or motor imagery.
    // Suppression of mu power can be indicative of motor cortex activation.
    // This could potentially be the most important one
    double muMin = 8.0; // Lower bound of mu band (Hz)
    double muMax = 12.0; // Upper bound of mu band (Hz)
    // Define the number of bins for discretization (for Shannon entropy)
    int numBins = 8; // Dividing in 8 bins of 128 values each
    // Define some STFT parameters
    int windowSize = 128; // This value specifies how much data you want to considering 128 bits at a time.
    int hopSize = 64; // Analysis window shifts 64 values after each analysis (less than the window sizeto have overlapping windows - which can help capture temporal or spatial information more effectively).
    
    // Initialize features struct
    Features features;

    // Start the analysis
    // Calculate statistical features
    StatisticalFeatures statisticalFeatures = calculateStatisticalFeatures(buffer);
    // Calculate FFTW and get the result
    std::vector<std::complex<double>> fftwResult = computeFFTW(buffer, N);
    // Calculate relative powers
    RelativePowers fftwRelativePowers = calculateRelativePowers(fftwResult, N, deltaMin, deltaMax, thetaMin, thetaMax, alphaMin, alphaMax, betaMin, betaMax, gammaMin, gammaMax, muMin, muMax);
    // Calculate the Shannon entropy
    double entropy = calculateShannonEntropy(buffer, numBins, N);
    // Calculate the zero-crossing rate
    int zeroCrossingRate = calculateZeroCrossingRate(buffer, N);
    // Calculate Hjorth parameters
    HjorthParameters hjorthParams = calculateHjorthParameters(buffer, N);
    // Calculate STFT powers
    std::vector<RelativePowers> stftRelativePowers = calculateSTFTRelativePowers(buffer, windowSize, hopSize, N, deltaMin, deltaMax, thetaMin, thetaMax, alphaMin, alphaMax, betaMin, betaMax, gammaMin, gammaMax, muMin, muMax);

    // Store in features struct
    features.numSamples = N;
    features.statisticalFeatures = statisticalFeatures;
    features.fftwRelativePowers = fftwRelativePowers;
    features.entropy = entropy;
    features.zeroCrossingRate = zeroCrossingRate;
    features.hjorthParams = hjorthParams;
    features.stftRelativePowers = stftRelativePowers;

    // Return the resulting features
    return features;
}