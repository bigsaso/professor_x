// shannon_entropy.cpp
#include "shannon_entropy.h"
#include <algorithm>
#include <cmath>

// Function to calculate Shannon entropy
double calculateShannonEntropy(const std::vector<double> signalData, int numBins, int N) {
    // Calculate min and max values in the signal data
    double minVal = *std::min_element(signalData.begin(), signalData.end());
    double maxVal = *std::max_element(signalData.begin(), signalData.end());

    // Calculate bin width
    double binWidth = (maxVal - minVal) / numBins;

    // Create a histogram to count data points in each bin
    std::map<int, int> histogram;

    // Populate the histogram
    for (int i=0;i<N;i++) {
        int binIndex = static_cast<int>((signalData[i] - minVal) / binWidth);
        histogram[binIndex]++;
    }

    // Calculate probability distribution and Shannon entropy
    double entropy = 0.0;
    for (const auto& entry : histogram) {
        double probability = static_cast<double>(entry.second) / N;
        if (probability > 0.0) {
            entropy -= probability * log2(probability);
        }
    }

    return entropy;
}
