// stft_relative_powers.h
#ifndef STFT_RELATIVE_POWERS_H
#define STFT_RELATIVE_POWERS_H

#include <vector>
#include <complex>
#include "fftw_relative_powers.h"

// Declaration of calculateSTFTRelativePowers function
std::vector<RelativePowers> calculateSTFTRelativePowers(const std::vector<double> signalData, 
                                                        int windowSize, int hopSize, int N, 
                                                        double deltaMin, double deltaMax, double thetaMin, 
                                                        double thetaMax, double alphaMin, double alphaMax, 
                                                        double betaMin, double betaMax, double gammaMin, 
                                                        double gammaMax, double muMin, double muMax);

#endif