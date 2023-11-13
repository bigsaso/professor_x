// higher_order_statistics.cpp
#include "higher_order_statistics.h"
#include <numeric>
#include <cmath>

std::vector<std::complex<double>> computeBispectrum(const std::vector<std::complex<double>> fftw, int N) {
    std::vector<std::complex<double>> bispectrum(N * N);

    for (int f1 = 0; f1 < N; ++f1) {
        for (int f2 = 0; f2 < N; ++f2) {
            int f3 = (f1 + f2) % N;
            bispectrum[f1 * N + f2] = fftw[f1] * fftw[f2] * std::conj(fftw[f3]);
        }
    }

    return bispectrum;
}

double computeThirdMoment(const std::vector<double> signal, int N) {
    double mean = std::accumulate(signal.begin(), signal.end(), 0.0) / N;
    double thirdMoment = 0.0;

    for (double value : signal) {
        thirdMoment += std::pow(value - mean, 3);
    }

    return thirdMoment / N;
}

HigherOrderStatistics computeHigherOrderStatistics(const std::vector<std::complex<double>> fftw, const std::vector<double> signal, int N, double deltaMin, double deltaMax, double thetaMin, double thetaMax, double alphaMin, double alphaMax, double betaMin, double betaMax, double gammaMin, double gammaMax, double muMin, double muMax) {
    HigherOrderStatistics higherOrderStatistics;
    // Calculate Higher Order Statistics
    std::vector<std::complex<double>> bispectrum = computeBispectrum(fftw, N);
    RelativePowers bispectrumRelativePowers = calculateRelativePowers(bispectrum, N, deltaMin, deltaMax, thetaMin, thetaMax, alphaMin, alphaMax, betaMin, betaMax, gammaMin, gammaMax, muMin, muMax);
    double thirdMoment = computeThirdMoment(signal, N);

    higherOrderStatistics.bispectrumRelativePowers = bispectrumRelativePowers;
    higherOrderStatistics.thirdMoment = thirdMoment;

    return higherOrderStatistics;
}