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

// Create struct for statistical features
struct StatisticalFeatures{
    double mean;
    double variance;
    double skewness;
    double kurtosis;
};

// Create struct for relative powers
struct RelativePowers{
    double relativeDeltaPower;
    double relativeThetaPower;
    double relativeAlphaPower;
    double relativeBetaPower;
    double relativeGammaPower;
    double relativeMuPower;
};

// Create struct for Hjorth parameters
struct HjorthParameters {
    double activity;
    double mobility;
    double complexity;
};

// Create struct for each buffer analysis features
struct Features {
    int numSamples; // Number of samples in current buffer
    StatisticalFeatures statisticalFeatures;
    RelativePowers fftwRelativePowers;
    double entropy;
    int zeroCrossingRate;
    HjorthParameters hjorthParams;
    std::vector<RelativePowers> stftRelativePowers; // This is a vector as there are 15 sliding windows being analyzed for each buffer
};

// Function to calculate statistical features
StatisticalFeatures calculateStatisticalFeatures(const std::vector<double> signalData){
    StatisticalFeatures statisticalFeatures;
    // Calculate mean
    double sum = 0.0;
    for (const double& value : signalData) {
        sum += value;
    }
    double mean = sum / signalData.size();
    // Calculate variance
    double variance = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        variance += deviation * deviation;
    }
    variance /= signalData.size();
    // Calculate skewness
    double skewness = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        skewness += (deviation * deviation * deviation);
    }
    skewness /= (signalData.size() * std::pow(variance, 1.5));
    // Calculate kurtosis
    double kurtosis = 0.0;
    for (const double& value : signalData) {
        double deviation = value - mean;
        kurtosis += (deviation * deviation * deviation * deviation);
    }
    kurtosis /= (signalData.size() * variance * variance);
    statisticalFeatures.mean = mean;
    statisticalFeatures.variance = variance;
    statisticalFeatures.skewness = skewness;
    statisticalFeatures.kurtosis = kurtosis;

    return statisticalFeatures;
}

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

// Function to calculate relative power
double calculateRelativePower(const std::vector<std::complex<double>> signal, double min, double max, int signalSize) {
    // Calculate power spectrum (magnitude squared of FFT)
    std::vector<double> powerSpectrum(signalSize);
    for(int i=0;i<signalSize;i++){
        powerSpectrum[i] = std::norm(signal[i]);
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
double calculateShannonEntropy(const std::vector<double> signalData, int numBins, int N) {
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
int calculateZeroCrossingRate(const std::vector<double> signalData, int N) {
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
HjorthParameters calculateHjorthParameters(const std::vector<double> signalData, int N) {
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

// Function to calculate FFTW Relative Powers
RelativePowers calculateRelativePowers(const std::vector<std::complex<double>> signal, int N, double deltaMin, double deltaMax, double thetaMin, double thetaMax, double alphaMin, double alphaMax, double betaMin, double betaMax, double gammaMin, double gammaMax, double muMin, double muMax) {
    RelativePowers relativePowers;
    // Calculate relative delta power
    double relativeDeltaPower = calculateRelativePower(signal, deltaMin, deltaMax, N);
    // Calculate relative theta power
    double relativeThetaPower = calculateRelativePower(signal, thetaMin, thetaMax, N);
    // Calculate relative alpha power
    double relativeAlphaPower = calculateRelativePower(signal, alphaMin, alphaMax, N);
    // Calculate relative beta power
    double relativeBetaPower = calculateRelativePower(signal, betaMin, betaMax, N);
    // Calculate relative gamma power
    double relativeGammaPower = calculateRelativePower(signal, gammaMin, gammaMax, N);
    // Calculate relative mu power
    double relativeMuPower = calculateRelativePower(signal, muMin, muMax, N);
    relativePowers.relativeDeltaPower = relativeDeltaPower;
    relativePowers.relativeThetaPower = relativeThetaPower;
    relativePowers.relativeAlphaPower = relativeAlphaPower;
    relativePowers.relativeBetaPower = relativeBetaPower;
    relativePowers.relativeGammaPower = relativeGammaPower;
    relativePowers.relativeMuPower = relativeMuPower;

    return relativePowers;
}

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

// Dummy analyze function, replace with actual analysis
Features analyze(const std::vector<double> buffer) {

    // Calculate size of the signal
    int N = buffer.size();

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
    int numBins = 8; // Dividing in 8 bins of 128 values each
    // Define some STFT parameters
    int windowSize = 128; // This value specifies how much data you want to considering 128 bits at a time.
    int hopSize = 64; // Analysis window shifts 64 values after each analysis (less than the window sizeto have overlapping windows - which can help capture temporal or spatial information more effectively).
    
    // Initialize features struct
    Features features;

    // Start the analysis
    // Calculate statistical features
    StatisticalFeatures statisticalFeatures = calculateStatisticalFeatures(buffer);
    // Calculate FFTW and get the result
    std::vector<std::complex<double>> fftwResult = computeFFTW(buffer, N);
    // Calculate relative powers
    RelativePowers fftwRelativePowers = calculateRelativePowers(fftwResult, N, deltaMin, deltaMax, thetaMin, thetaMax, alphaMin, alphaMax, betaMin, betaMax, gammaMin, gammaMax, muMin, muMax);
    // Calculate the Shannon entropy
    double entropy = calculateShannonEntropy(buffer, numBins, N);
    // Calculate the zero-crossing rate
    int zeroCrossingRate = calculateZeroCrossingRate(buffer, N);
    // Calculate Hjorth parameters
    HjorthParameters hjorthParams = calculateHjorthParameters(buffer, N);
    // Calculate STFT powers
    std::vector<RelativePowers> stftRelativePowers = calculateSTFTRelativePowers(buffer, windowSize, hopSize, N, deltaMin, deltaMax, thetaMin, thetaMax, alphaMin, alphaMax, betaMin, betaMax, gammaMin, gammaMax, muMin, muMax);

    // Store in features struct
    features.numSamples = N;
    // features.mean = mean;
    // features.variance = variance;
    // features.skewness = skewness;
    // features.kurtosis = kurtosis;
    features.statisticalFeatures = statisticalFeatures;
    features.fftwRelativePowers = fftwRelativePowers;
    features.entropy = entropy;
    features.zeroCrossingRate = zeroCrossingRate;
    features.hjorthParams = hjorthParams;
    features.stftRelativePowers = stftRelativePowers;

    // Return the resulting features
    return features;
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

    // Define buffer size as 1024 (2s of sampling data at 512Hz)
    const int bufferSize = 1024;
    // Initialize the buffer
    std::vector<double> buffer;
    // Initialize a map of analysisResults
    std::map<std::string, Features> analysisResults;
    // Initialize counter -- will be used to identify each bucket analyzed
    int counter = 1;

    // Analyze signal in chunks
    for (double value : signalData) {
        // Add signal values to the buffer
        buffer.push_back(value);
        // Once buffer is full
        if (buffer.size() == bufferSize) {
            // Analyze it
            Features features = analyze(buffer);
            // Store results in their respective bucket
            analysisResults["buffer" + std::to_string(counter) + "_analyzed"] = features;
            // Then clear the buffer
            buffer.clear();
            // And increment the counter
            ++counter;
        }
    }

    // Analyze remaining data in buffer (if any and less than 1024)
    if (!buffer.empty()) {
        // Analyze it
        Features features = analyze(buffer);
        // Store results in their respective bucket
        analysisResults["buffer" + std::to_string(counter) + "_analyzed"] = features;
        // Then clear the buffer
        buffer.clear();
        // And increment the counter
        ++counter;
    }

    // Output results
    for (const auto& [name, features] : analysisResults) {
        int frameNum = 1; // Initialize frame number
        std::cout << name << ": "
                << "Samples in buffer = " << features.numSamples
                << "\n\tMean = " << features.statisticalFeatures.mean
                << "\n\tVariance = " << features.statisticalFeatures.variance
                << "\n\tSkewness = " << features.statisticalFeatures.skewness
                << "\n\tKurtosis = " << features.statisticalFeatures.kurtosis
                << "\n\tRelative Delta Power = " << features.fftwRelativePowers.relativeDeltaPower
                << "\n\tRelative Theta Power = " << features.fftwRelativePowers.relativeThetaPower
                << "\n\tRelative Alpha Power = " << features.fftwRelativePowers.relativeAlphaPower
                << "\n\tRelative Beta Power = " << features.fftwRelativePowers.relativeBetaPower
                << "\n\tRelative Gamma Power = " << features.fftwRelativePowers.relativeGammaPower
                << "\n\tRelative Mu Power = " << features.fftwRelativePowers.relativeMuPower
                << "\n\tEntropy = " << features.entropy
                << "\n\tZero Crossing Rate = " << features.zeroCrossingRate
                << "\n\tHjorth Parameters:"
                << "\n\t\tActivity = " << features.hjorthParams.activity
                << "\n\t\tMobility = " << features.hjorthParams.mobility
                << "\n\t\tComplexity = " << features.hjorthParams.complexity
                // << std::endl;
                << "\n\tSTFT Relative Powers: [";
                for (const auto& stftRelativePowers : features.stftRelativePowers) {
                    std::cout << "\n\t\tFrame " << frameNum << "("
                            << "Delta = " << stftRelativePowers.relativeDeltaPower
                            << ", Theta = " << stftRelativePowers.relativeThetaPower
                            << ", Alpha = " << stftRelativePowers.relativeAlphaPower
                            << ", Beta = " << stftRelativePowers.relativeBetaPower
                            << ", Gamma = " << stftRelativePowers.relativeGammaPower
                            << ", Mu = " << stftRelativePowers.relativeMuPower
                            << "),";
                    frameNum++; // Increment frame number
                }
                std::cout << "\n\t]" << std::endl;
    }

    return 0;
}
