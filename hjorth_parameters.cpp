// hjorth_parameters.cpp
#include "hjorth_parameters.h"

// Function to calculate Hjorth parameters
HjorthParameters calculateHjorthParameters(const std::vector<double> signalData, int N) {
    HjorthParameters hjorth;

    // Calculate Activity
    double activity = 0.0;
    for (int i=0;i<N;i++) {
        activity += signalData[i] * signalData[i];
    }
    hjorth.activity = activity / N;

    // Calculate Mobility
    double mobility = 0.0;
    for (int i=0;i<N;i++) {
        mobility += pow(signalData[i] - signalData[i - 1], 2);
    }
    mobility = sqrt(mobility / (N - 1)) / sqrt(hjorth.activity);
    hjorth.mobility = mobility;

    // Calculate Complexity
    double complexity = 0.0;
    for (int i=0;i<N;i++) {
        complexity += pow(signalData[i] - 2 * signalData[i - 1] + signalData[i - 2], 2);
    }
    complexity = sqrt(complexity / (N - 1)) / mobility;
    hjorth.complexity = complexity;

    return hjorth;
}