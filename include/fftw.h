// fftw.h
#ifndef FFTW_H
#define FFTW_H

#include <vector>
#include <complex>

std::vector<std::complex<double>> computeFFTW(const std::vector<double> signalData, int N);

#endif