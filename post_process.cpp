#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <complex>
#include <fftw3.h>
#include <map>
#include <algorithm>

using namespace std;

// Create struct for Hjorth parameters
struct HjorthParameters {
    double activity;
    double mobility;
    double complexity;
};

// Function to compute mean of the signal
double computeMean(const std::vector<double>& signalData) {
    double sum = 0.0;
    for (const double& value : signalData) {
        sum += value;
    }
    double mean = sum / signalData.size();
    return mean;
}

// Function to compute variance of the signal
double computeVariance(const std::vector<double>& signalData, double mean) {
    double variance = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        variance += deviation * deviation;
    }
    variance /= signalData.size();
    return variance;
}

// Function to compute skewness of the signal
double computeSkewness(const std::vector<double>& signalData, double mean, double variance) {
    double skewness = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        skewness += (deviation * deviation * deviation);
    }
    skewness /= (signalData.size() * std::pow(variance, 1.5));
    return skewness;
}

// Function to compute kurtosis of the signal
double computeKurtosis(const std::vector<double>& signalData, double mean, double variance) {
    double kurtosis = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        kurtosis += (deviation * deviation * deviation * deviation);
    }
    kurtosis /= (signalData.size() * variance * variance);
    return kurtosis;
}

// Function to compute FFTW of the signal
std::vector<std::complex<double>> computeFFTW(const std::vector<double>& signalData, int N) {
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

// Function to store FFTW of the signal in .csv file
void storeFFTWtoCSV(std::vector<std::complex<double>> fftwResult, int signalSize){
    std::ofstream outputFile("fftw.csv");
    if(outputFile.is_open()){
            for(int i=0;i<signalSize;i++){
                    outputFile << fftwResult[i] << "\n";
            }
            outputFile.close();
            std::cout << "FFTW output written to fftw.csv successfully" << std::endl;
    }else{
        std:cerr << "Failed to open fftw.csv for writing." << std::endl;
    }
}

// Function to calculate relative power
double calculateRelativePower(std::vector<std::complex<double>> fftwResult, double min, double max, int signalSize) {
    // Calculate power spectrum (magnitude squared of FFT)
    std::vector<double> powerSpectrum(signalSize);
    for(int i=0;i<signalSize;i++){
        powerSpectrum[i] = std::norm(fftwResult[i]);
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

// Function to calculate Shannon entropy
double calculateShannonEntropy(const std::vector<double>& signalData, int numBins, int N) {
    // Calculate min and max values in the signal data
    double minVal = *std::min_element(signalData.begin(), signalData.end());
    double maxVal = *std::max_element(signalData.begin(), signalData.end());

    // Calculate bin width
    double binWidth = (maxVal - minVal) / numBins;

    // Create a histogram to count data points in each bin
    std::map<int, int> histogram;

    // Populate the histogram
    for (int i=0;i<N;i++) {
        int binIndex = static_cast<int>((signalData[i] - minVal) / binWidth);
        histogram[binIndex]++;
    }

    // Calculate probability distribution and Shannon entropy
    double entropy = 0.0;
    for (const auto& entry : histogram) {
        double probability = static_cast<double>(entry.second) / N;
        if (probability > 0.0) {
            entropy -= probability * log2(probability);
        }
    }

    return entropy;
}

// Function to calculate zero-crossing rate
int calculateZeroCrossingRate(const std::vector<double>& signalData, int N) {
    int zeroCrossings = 0;
    for (int i=1;i<N;i++) {
        // Check if the current value has a different sign than the previous value
        if ((signalData[i] >= 0 && signalData[i - 1] < 0) || (signalData[i] < 0 && signalData[i - 1] >= 0)) {
            zeroCrossings++;
        }
    }
    return zeroCrossings;
}

// Function to calculate Hjorth parameters
HjorthParameters calculateHjorthParameters(const std::vector<double>& signalData, int N) {
    HjorthParameters hjorth;

    // Calculate Activity
    double activity = 0.0;
    for (int i=0;i<N;i++) {
        activity += signalData[i] * signalData[i];
    }
    hjorth.activity = activity / N;

    // Calculate Mobility
    double mobility = 0.0;
    for (int i=0;i<N;i++) {
        mobility += pow(signalData[i] - signalData[i - 1], 2);
    }
    mobility = sqrt(mobility / (N - 1)) / sqrt(hjorth.activity);
    hjorth.mobility = mobility;

    // Calculate Complexity
    double complexity = 0.0;
    for (int i=0;i<N;i++) {
        complexity += pow(signalData[i] - 2 * signalData[i - 1] + signalData[i - 2], 2);
    }
    complexity = sqrt(complexity / (N - 1)) / mobility;
    hjorth.complexity = complexity;

    return hjorth;
}

// Function to perform Short-Time Fourier Transform (STFT)
std::vector<std::vector<std::complex<double>>> calculateSTFT(const std::vector<double>& signalData, int windowSize, int hopSize, int N) {
    std::vector<std::vector<std::complex<double>>> stftResult;

    // Initialize FFTW plans
    fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * windowSize);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * windowSize);
    fftw_plan plan = fftw_plan_dft_1d(windowSize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    int numFrames = (N - windowSize) / hopSize + 1;

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

        stftResult.push_back(stftFrame);
    }

    // Clean up FFTW resources
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return stftResult;
}

int main() {

    // Initialize a vector to store eeg signal data
    std::vector<double> signalData;
    // Read from pre-processed signal in .csv file
    std::ifstream inputFile("preprocessed_signal.csv");
    // Store in the vector initialized earlier
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
    // Calculate size of the signal
    int N = signalData.size();

    // Define frequency bands (for relative power)
    double deltaMin = 0.5; // Lower bound of delta band (Hz)
    double deltaMax = 4.0; // Upper bound of delta band (Hz)
    double thetaMin = 4.0; // Lower bound of theta band (Hz)
    double thetaMax = 8.0; // Upper bound of theta band (Hz)
    double alphaMin = 8.0; // Lower bound of alpha band (Hz)
    double alphaMax = 13.0; // Upper bound of alpha band (Hz)
    double betaMin = 13.0; // Lower bound of beta band (Hz)
    double betaMax = 30.0; // Upper bound of beta band (Hz)
    double gammaMin = 30.0; // Lower bound of gamma band (Hz)
    double gammaMax = 160.0; // Upper bound of gamma band (Hz). This one will change based on headset's frequency.
    // Mu rhythm is associated with the sensorimotor cortex and can be attenuated during motor tasks or motor imagery.
    // Suppression of mu power can be indicative of motor cortex activation.
    // This could potentially be the most important one
    double muMin = 8.0; // Lower bound of mu band (Hz)
    double muMax = 12.0; // Upper bound of mu band (Hz)
    // Define the number of bins for discretization (for Shannon entropy)
    int numBins = 10; // This value can be adjusted as needed
    // Define some STFT parameters
    int windowSize = 256; // This value can be adjusted as needed
    int hopSize = 64; // This value can be adjusted as needed

    // Calculate the mean
    double mean = computeMean(signalData);
    // Calculate the variance
    double variance = computeVariance(signalData, mean);
    // Calculate skewness
    double skewness = computeSkewness(signalData, mean, variance);
    // Calculate kurtosis
    double kurtosis = computeKurtosis(signalData, mean, variance);
    // Calculate FFTW and get the result
    std::vector<std::complex<double>> fftwResult = computeFFTW(signalData, N);
    // Calculate relative delta power
    double relativeDeltaPower = calculateRelativePower(fftwResult, deltaMin, deltaMax, N);
    // Calculate relative theta power
    double relativeThetaPower = calculateRelativePower(fftwResult, thetaMin, thetaMax, N);
    // Calculate relative alpha power
    double relativeAlphaPower = calculateRelativePower(fftwResult, alphaMin, alphaMax, N);
    // Calculate relative beta power
    double relativeBetaPower = calculateRelativePower(fftwResult, betaMin, betaMax, N);
    // Calculate relative gamma power
    double relativeGammaPower = calculateRelativePower(fftwResult, gammaMin, gammaMax, N);
    // Calculate relative mu power
    double relativeMuPower = calculateRelativePower(fftwResult, muMin, muMax, N);
    // Calculate the Shannon entropy
    double entropy = calculateShannonEntropy(signalData, numBins, N);
    // Calculate the zero-crossing rate
    int zeroCrossingRate = calculateZeroCrossingRate(signalData, N);
    // Calculate Hjorth parameters
    HjorthParameters params = calculateHjorthParameters(signalData, N);
    // Calculate STFT and get the result
    std::vector<std::vector<std::complex<double>>> stftResult = calculateSTFT(signalData, windowSize, hopSize, N);

    // Debug: Print all values
    std::cout << "Mean of the signal: " << mean << std::endl;
    std::cout << "Variance of the signal: " << variance << std::endl;
    std::cout << "Skewness of the signal: " << skewness << std::endl;
    std::cout << "Kurtosis of the signal: " << kurtosis << std::endl;
    storeFFTWtoCSV(fftwResult, N);
    std::cout << "Relative Delta Power: " << relativeDeltaPower << std::endl;
    std::cout << "Relative Theta Power: " << relativeThetaPower << std::endl;
    std::cout << "Relative Alpha Power: " << relativeAlphaPower << std::endl;
    std::cout << "Relative Beta Power: " << relativeAlphaPower << std::endl;
    std::cout << "Relative Gamma Power: " << relativeGammaPower << std::endl;
    std::cout << "Relative Mu Power: " << relativeMuPower << std::endl;
    std::cout << "Shannon Entropy: " << entropy << std::endl;
    std::cout << "Zero-Crossing Rate: " << zeroCrossingRate << std::endl;
    std::cout << "Hjorth parameters - Activity: " << params.activity << " - Mobility: " << params.mobility << " - Complexity: " << params.complexity << std::endl;

    return 0;
    
}
