#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <complex>
#include <fftw3.h>

using namespace std;

int main() {
    std::vector<double> signalData;
    std::ifstream inputFile("preprocessed_signal.csv");

    if (inputFile.is_open()) {
        double value;
        while (inputFile >> value) {
            signalData.push_back(value);
        }
        inputFile.close();
    } else {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    // Compute the mean of the signal
    double sum = 0.0;
    for (const double& value : signalData) {
        sum += value;
    }
    double mean = sum / signalData.size();

    // Compute the variance of the signal
    double variance = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        variance += deviation * deviation;
    }
    variance /= signalData.size();

    // Compute the skewness of the signal
    double skewness = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        skewness += (deviation * deviation * deviation);
    }
    skewness /= (signalData.size() * std::pow(variance, 1.5));

    // Compute the kurtosis of the signal
    double kurtosis = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        kurtosis += (deviation * deviation * deviation * deviation);
    }
    kurtosis /= (signalData.size() * variance * variance);

    // Compute FFTW of the signal
    int N = signalData.size();
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < N; i++) {
        in[i][0] = signalData[i]; // Real part
        in[i][1] = 0.0;       // Imaginary part (set to 0 for real data)
    }

    fftw_execute(plan);

    // Store output to .csv file
    std::ofstream outputFile("fftw.csv");
    if(outputFile.is_open()){
            for(int i=0;i<N;i++){
                    // Write the real and imaginary parts of each element to the file, separated by a comma
                    outputFile << out[i][0] << "," << out[i][1] << "\n";
            }
            outputFile.close();
            std::cout << "FFTW output written to fftw.csv successfully" << std::endl;
    }else{
        std:cerr << "Failed to open fftw.csv for writing." << std::endl;
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    std::cout << "Mean of the signal: " << mean << std::endl;
    std::cout << "Variance of the signal: " << variance << std::endl;
    std::cout << "Skewness of the signal: " << skewness << std::endl;
    std::cout << "Kurtosis of the signal: " << kurtosis << std::endl;

    return 0;
}
