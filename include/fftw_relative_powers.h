 // fftw_relative_powers.h
#ifndef FFTW_RELATIVE_POWERS_H
#define FFTW_RELATIVE_POWERS_H

#include <vector>
#include <complex>

// Struct declaration
struct RelativePowers {
    double relativeDeltaPower;
    double relativeThetaPower;
    double relativeAlphaPower;
    double relativeBetaPower;
    double relativeGammaPower;
    double relativeMuPower;
};

// Function declarations
double calculateRelativePower(const std::vector<std::complex<double>> fftw, double min, double max, int signalSize);
RelativePowers calculateRelativePowers(const std::vector<std::complex<double>> fftw, int N, 
                                       double deltaMin, double deltaMax, double thetaMin, double thetaMax, 
                                       double alphaMin, double alphaMax, double betaMin, double betaMax, 
                                       double gammaMin, double gammaMax, double muMin, double muMax);

#endif
