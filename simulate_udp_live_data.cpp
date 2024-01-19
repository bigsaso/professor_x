#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "features_analysis.h"

using namespace std;

int main(int argc, char* argv[]) {
    
    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error opening socket" << std::endl;
        return 1;
    }

    // Prepare the sockaddr_in structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT); // Port number

    // Bind
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    double buffer[1024];

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

    Features features;
    while (true) {
        std::cout << "Waiting for data..." << std::endl;

        if (receiveData(sockfd, client_addr, buffer, 1024)) {
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
                    << "\n\tStatistical Features: ["
                    << "\n\t\tMean = " << features.statisticalFeatures.mean
                    << "\n\t\tVariance = " << features.statisticalFeatures.variance
                    << "\n\t\tSkewness = " << features.statisticalFeatures.skewness
                    << "\n\t\tKurtosis = " << features.statisticalFeatures.kurtosis
                    << "\n\t]"
                    << "\n\tFFTW Relative Powers: ["
                    << "\n\t\tRelative Delta Power = " << features.fftwRelativePowers.relativeDeltaPower
                    << "\n\t\tRelative Theta Power = " << features.fftwRelativePowers.relativeThetaPower
                    << "\n\t\tRelative Alpha Power = " << features.fftwRelativePowers.relativeAlphaPower
                    << "\n\t\tRelative Beta Power = " << features.fftwRelativePowers.relativeBetaPower
                    << "\n\t\tRelative Gamma Power = " << features.fftwRelativePowers.relativeGammaPower
                    << "\n\t\tRelative Mu Power = " << features.fftwRelativePowers.relativeMuPower
                    << "\n\t]"
                    << "\n\tEntropy = " << features.entropy
                    << "\n\tZero Crossing Rate = " << features.zeroCrossingRate
                    << "\n\tHjorth Parameters: ["
                    << "\n\t\tActivity = " << features.hjorthParams.activity
                    << "\n\t\tMobility = " << features.hjorthParams.mobility
                    << "\n\t\tComplexity = " << features.hjorthParams.complexity
                    << "\n\t]"
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
                    std::cout << "\n\t]"
                    << "\n\tBispectrum Relative Powers: ["
                    << "\n\t\tRelative Delta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeDeltaPower
                    << "\n\t\tRelative Theta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeThetaPower
                    << "\n\t\tRelative Alpha Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeAlphaPower
                    << "\n\t\tRelative Beta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeBetaPower
                    << "\n\t\tRelative Gamma Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeGammaPower
                    << "\n\t\tRelative Mu Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeMuPower
                    << "\n\t]"
                    << "\n\tThird Moment: " << features.higherOrderStatistics.thirdMoment
                    << std::endl;
                // Then clear the buffer
                buffer.clear();
                // And increment the counter
                ++counter;
            }
            // Add a delay to simulate live data at 512Hz
            this_thread::sleep_for(chrono::microseconds(static_cast<int>(delayMilliseconds * 1000.0)));
        }
    }

    return 0;
}