// stft_relative_powers.cpp
#include "stft_relative_powers.h"
#include <fftw3.h>

// Function to calculate STFT Relative Powers
std::vector<RelativePowers> calculateSTFTRelativePowers(const std::vector<double> signalData, int windowSize, int hopSize, int N, double deltaMin, double deltaMax, double thetaMin, double thetaMax, double alphaMin, double alphaMax, double betaMin, double betaMax, double gammaMin, double gammaMax, double muMin, double muMax) {

    std::vector<RelativePowers> results;
    // Initialize FFTW plans
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * windowSize);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * windowSize);
    fftw_plan plan = fftw_plan_dft_1d(windowSize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    int numFrames = ((N - windowSize) / hopSize) + 1;

    for (int frame=0;frame<numFrames;frame++) {

        std::vector<std::complex<double>> stftFrame;

        // Fill the input buffer with the current frame
        for (int i=0;i<windowSize;i++) {
            in[i][0] = signalData[frame * hopSize + i];
            in[i][1] = 0.0; // Imaginary part is zero
        }

        // Perform FFT
        fftw_execute(plan);

        // Copy FFT result to the STFT frame
        for (int i=0;i<windowSize;i++) {
            stftFrame.push_back(std::complex<double>(out[i][0], out[i][1]));
        }

        // Calculate powers
        RelativePowers powers = calculateRelativePowers(stftFrame, N, deltaMin, deltaMax, thetaMin, thetaMax, alphaMin, alphaMax, betaMin, betaMax, gammaMin, gammaMax, muMin, muMax);

        // Add current frame's powers in the vector
        results.push_back(powers);
        // Clear the stftFrame to move on to next frame
        stftFrame.clear();
    }

    // Clean up FFTW resources
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return results;
}