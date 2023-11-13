#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "features_analysis.h"

using namespace std;

int main(int argc, char* argv[]) {
    
    // Declare the ifstream outside of the if block
    std::ifstream csvFile;
    if (argc < 2) {
        // Read from pre-processed signal in .csv file
        csvFile.open("../preprocessed_signal.csv");
        std::cout << "Processing preprocessed_signal.csv" << std::endl;
    }
    else{
        // Read from pre-processed signal in .csv file
        csvFile.open(argv[1]);
        std::cout << "Processing " << argv[1] << std::endl;
    }

    if (!csvFile.is_open()) {
        cerr << "Failed to open CSV file." << endl;
        return 1;
    }

    // Calculate the time delay between each data point to achieve 512Hz
    double samplingRateHz = 512.0;
    double delayMilliseconds = 1000.0 / samplingRateHz;

    // Define buffer size as 1024 (2s of sampling data at 512Hz)
    const int bufferSize = 1024;
    // Initialize the buffer
    std::vector<double> buffer;
    // Initialize a map of analysisResults
    std::map<std::string, Features> analysisResults;
    // Initialize counter -- will be used to identify each bucket analyzed
    int counter = 1;
    bool isFirstBuffer = true;

    string data;
    Features features;
    while (getline(csvFile, data)) {
        // You can process the data point here or simply ignore it
        buffer.push_back(stod(data));
        // Once buffer is full
        if (buffer.size() == bufferSize && isFirstBuffer) {
            // Analyze it
            features = analyze(buffer);
            // Clear the buffer
            buffer.clear();
            // Increment the counter
            ++counter;
            // And disable isFirstBuffer
            isFirstBuffer = false;
        }
        else if (buffer.size() == bufferSize && !isFirstBuffer) {
            // Analyze it
            features = analyze(buffer);
            // Store results in their respective bucket
            std::cout << "buffer" << std::to_string(counter) << "_analyzed" << std::endl;
            // Display results
            int frameNum = 1; // Initialize frame number
            std::cout << "Samples in buffer = " << features.numSamples
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
                std::cout << "\n\t]"// << std::endl;
                << "\n\tBispectrum Relative Powers: ["
                << "\n\t\tRelative Delta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeDeltaPower
                << "\n\t\tRelative Theta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeThetaPower
                << "\n\t\tRelative Alpha Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeAlphaPower
                << "\n\t\tRelative Beta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeBetaPower
                << "\n\t\tRelative Gamma Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeGammaPower
                << "\n\t\tRelative Mu Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeMuPower
                << "\n\t]"
                << "\n\tThird Moment: " << features.higherOrderStatistics.thirdMoment << std::endl;
            // Then clear the buffer
            buffer.clear();
            // And increment the counter
            ++counter;
        }
        // Add a delay to simulate live data at 512Hz
        this_thread::sleep_for(chrono::microseconds(static_cast<int>(delayMilliseconds * 1000.0)));
    }

    // Close the CSV file
    csvFile.close();

    return 0;
}