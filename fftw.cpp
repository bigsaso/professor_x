// fftw.cpp
#include "fftw.h"
#include <fftw3.h>

// Function to compute FFTW of the signal
std::vector<std::complex<double>> computeFFTW(const std::vector<double> signalData, int N) {
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i=0;i<N;i++) {
        in[i][0] = signalData[i]; // Real part
        in[i][1] = 0.0;           // Imaginary part (set to 0 for real data)
    }

    // Perform FFTW
    fftw_execute(plan);

    std::vector<std::complex<double>> fftwResult;
    fftwResult.reserve(N);

    for (int i=0;i<N;i++) {
        std::complex<double> complexValue(out[i][0], out[i][1]);
        fftwResult.push_back(complexValue);
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return fftwResult;
}