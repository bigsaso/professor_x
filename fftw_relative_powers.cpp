// fftw_relative_powers.cpp
#include "fftw_relative_powers.h"

// Function to calculate relative power
double calculateRelativePower(const std::vector<std::complex<double>> fftw, double min, double max, int signalSize) {
    // Calculate power spectrum (magnitude squared of FFT)
    std::vector<double> powerSpectrum(signalSize);
    for(int i=0;i<signalSize;i++){
        powerSpectrum[i] = std::norm(fftw[i]);
    }
    // Calculate the total power (sum of all frequencies)
    double totalPower = 0.0;
    for (int i=0;i<signalSize;i++) {
        totalPower += powerSpectrum[i];
    }
    // Calculate the relative power in the band
    double power = 0.0;
    for (int i=int(min);i<=int(max);i++) {
        power += powerSpectrum[i];
    }
    // Calculate the relative power as a proportion of the total power
    double relativePower = power / totalPower;
    
    return relativePower;
}

// Function to calculate FFTW Relative Powers
RelativePowers calculateRelativePowers(const std::vector<std::complex<double>> fftw, int N, double deltaMin, double deltaMax, double thetaMin, double thetaMax, double alphaMin, double alphaMax, double betaMin, double betaMax, double gammaMin, double gammaMax, double muMin, double muMax) {
    RelativePowers relativePowers;
    // Calculate relative delta power
    double relativeDeltaPower = calculateRelativePower(fftw, deltaMin, deltaMax, N);
    // Calculate relative theta power
    double relativeThetaPower = calculateRelativePower(fftw, thetaMin, thetaMax, N);
    // Calculate relative alpha power
    double relativeAlphaPower = calculateRelativePower(fftw, alphaMin, alphaMax, N);
    // Calculate relative beta power
    double relativeBetaPower = calculateRelativePower(fftw, betaMin, betaMax, N);
    // Calculate relative gamma power
    double relativeGammaPower = calculateRelativePower(fftw, gammaMin, gammaMax, N);
    // Calculate relative mu power
    double relativeMuPower = calculateRelativePower(fftw, muMin, muMax, N);
    relativePowers.relativeDeltaPower = relativeDeltaPower;
    relativePowers.relativeThetaPower = relativeThetaPower;
    relativePowers.relativeAlphaPower = relativeAlphaPower;
    relativePowers.relativeBetaPower = relativeBetaPower;
    relativePowers.relativeGammaPower = relativeGammaPower;
    relativePowers.relativeMuPower = relativeMuPower;

    return relativePowers;
}
