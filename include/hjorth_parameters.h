// hjorth_parameters.h
#ifndef HJORTH_PARAMETERS_H
#define HJORTH_PARAMETERS_H

#include <vector>
#include <cmath>

// Struct declaration for HjorthParameters
struct HjorthParameters {
    double activity;
    double mobility;
    double complexity;
};

// Declaration of the calculateHjorthParameters function
HjorthParameters calculateHjorthParameters(const std::vector<double> signalData, int N);

#endif