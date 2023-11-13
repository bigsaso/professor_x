// higher_order_statistics.h
#ifndef HIGHER_ORDER_STATISTICS_H
#define HIGHER_ORDER_STATISTICS_H

#include "fftw_relative_powers.h"
#include <vector>
#include <complex>

struct HigherOrderStatistics {
    RelativePowers bispectrumRelativePowers;
    double thirdMoment;
};

std::vector<std::complex<double>> computeBispectrum(const std::vector<std::complex<double>> signal, int N);
double computeThirdMoment(const std::vector<double> signal, int N);
HigherOrderStatistics computeHigherOrderStatistics(const std::vector<std::complex<double>> fftw, const std::vector<double> signal, int N, double deltaMin, double deltaMax, double thetaMin, double thetaMax, double alphaMin, double alphaMax, double betaMin, double betaMax, double gammaMin, double gammaMax, double muMin, double muMax);

#endif