#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <tensorflow/lite/interpreter.h>
#include <tensorflow/lite/model.h>
#include <tensorflow/lite/kernels/register.h>
#include <iostream>
#include <vector>

#include "features_analysis.h"

#define PORT 8080

using namespace std;

bool receiveData(int sockfd, struct sockaddr_in &client_addr, double* buffer, int bufferSize) {
    socklen_t addr_len = sizeof(client_addr);
    int expectedSize = bufferSize * sizeof(double); // Size in bytes

    ssize_t recv_len = recvfrom(sockfd, (char*) buffer, expectedSize, 0, 
                                (struct sockaddr *) &client_addr, &addr_len);

    if (recv_len < 0) {
        std::cerr << "recvfrom failed" << std::endl;
        return false;
    } else if (recv_len != expectedSize) {
        std::cerr << "Received unexpected data size: " << recv_len << std::endl;
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    
    // Initialize UDP connection
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

    // Load the model
    auto model = tflite::FlatBufferModel::BuildFromFile("/home/pi/Documents/professor_x/machine_learning/tflite/model.tflite");

    // Build the interpreter
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    tflite::InterpreterBuilder(*model, resolver)(&interpreter);

    // Allocate tensors
    interpreter->AllocateTensors();

    // Get input and output tensors
    auto* input_tensor = interpreter->tensor(interpreter->inputs()[0]);
    auto* output_tensor = interpreter->tensor(interpreter->outputs()[0]);

    double raw_data[1024];

    // Calculate the time delay between each data point to achieve 512Hz
    double samplingRateHz = 512.0;
    double delayMilliseconds = 1000.0 / samplingRateHz;

    // Define buffer size as 1024 (2s of sampling data at 512Hz)
    const int bufferSize = 1024;
    // // Initialize the buffer
    // std::vector<double> buffer;
    // Initialize a map of analysisResults
    std::map<std::string, Features> analysisResults;
    // Initialize counter -- will be used to identify each bucket analyzed
    int counter = 1;
    bool isFirstBuffer = true;

    Features features;
    while (true) {
        std::cout << "Waiting for data..." << std::endl;

        if (receiveData(sockfd, client_addr, raw_data, 1024)) {
            // Initialize the buffer
            std::vector<double> buffer(raw_data, raw_data + 1024);
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
                // // Store results in their respective bucket
                // std::cout << "buffer" << std::to_string(counter) << "_analyzed" << std::endl;
                // // Display results
                // int frameNum = 1; // Initialize frame number
                // std::cout << "Samples in buffer = " << features.numSamples
                //     << "\n\tStatistical Features: ["
                //     << "\n\t\tMean = " << features.statisticalFeatures.mean
                //     << "\n\t\tVariance = " << features.statisticalFeatures.variance
                //     << "\n\t\tSkewness = " << features.statisticalFeatures.skewness
                //     << "\n\t\tKurtosis = " << features.statisticalFeatures.kurtosis
                //     << "\n\t]"
                //     << "\n\tFFTW Relative Powers: ["
                //     << "\n\t\tRelative Delta Power = " << features.fftwRelativePowers.relativeDeltaPower
                //     << "\n\t\tRelative Theta Power = " << features.fftwRelativePowers.relativeThetaPower
                //     << "\n\t\tRelative Alpha Power = " << features.fftwRelativePowers.relativeAlphaPower
                //     << "\n\t\tRelative Beta Power = " << features.fftwRelativePowers.relativeBetaPower
                //     << "\n\t\tRelative Gamma Power = " << features.fftwRelativePowers.relativeGammaPower
                //     << "\n\t\tRelative Mu Power = " << features.fftwRelativePowers.relativeMuPower
                //     << "\n\t]"
                //     << "\n\tEntropy = " << features.entropy
                //     << "\n\tZero Crossing Rate = " << features.zeroCrossingRate
                //     << "\n\tHjorth Parameters: ["
                //     << "\n\t\tActivity = " << features.hjorthParams.activity
                //     << "\n\t\tMobility = " << features.hjorthParams.mobility
                //     << "\n\t\tComplexity = " << features.hjorthParams.complexity
                //     << "\n\t]"
                //     << "\n\tSTFT Relative Powers: [";
                //     for (const auto& stftRelativePowers : features.stftRelativePowers) {
                //         std::cout << "\n\t\tFrame " << frameNum << "("
                //                 << "Delta = " << stftRelativePowers.relativeDeltaPower
                //                 << ", Theta = " << stftRelativePowers.relativeThetaPower
                //                 << ", Alpha = " << stftRelativePowers.relativeAlphaPower
                //                 << ", Beta = " << stftRelativePowers.relativeBetaPower
                //                 << ", Gamma = " << stftRelativePowers.relativeGammaPower
                //                 << ", Mu = " << stftRelativePowers.relativeMuPower
                //                 << "),";
                //         frameNum++; // Increment frame number
                //     }
                //     std::cout << "\n\t]"
                //     << "\n\tBispectrum Relative Powers: ["
                //     << "\n\t\tRelative Delta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeDeltaPower
                //     << "\n\t\tRelative Theta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeThetaPower
                //     << "\n\t\tRelative Alpha Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeAlphaPower
                //     << "\n\t\tRelative Beta Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeBetaPower
                //     << "\n\t\tRelative Gamma Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeGammaPower
                //     << "\n\t\tRelative Mu Power = " << features.higherOrderStatistics.bispectrumRelativePowers.relativeMuPower
                //     << "\n\t]"
                //     << "\n\tThird Moment: " << features.higherOrderStatistics.thirdMoment
                //     << std::endl;

                // Initiate the feature vector
                std::vector<float> feature_vector;
                // Add features to the vector in the correct order
                feature_vector.push_back(features.statisticalFeatures.mean);
                feature_vector.push_back(features.statisticalFeatures.skewness);
                feature_vector.push_back(features.statisticalFeatures.kurtosis);
                feature_vector.push_back(features.fftwRelativePowers.relativeThetaPower);
                feature_vector.push_back(features.fftwRelativePowers.relativeAlphaPower);
                feature_vector.push_back(features.fftwRelativePowers.relativeBetaPower);
                feature_vector.push_back(features.fftwRelativePowers.relativeGammaPower);
                feature_vector.push_back(features.fftwRelativePowers.relativeMuPower);
                feature_vector.push_back(features.entropy);
                feature_vector.push_back(features.hjorthParams.complexity);
                feature_vector.push_back(features.higherOrderStatistics.bispectrumRelativePowers.relativeThetaPower);
                feature_vector.push_back(features.higherOrderStatistics.bispectrumRelativePowers.relativeAlphaPower);
                feature_vector.push_back(features.higherOrderStatistics.bispectrumRelativePowers.relativeBetaPower);
                feature_vector.push_back(features.higherOrderStatistics.bispectrumRelativePowers.relativeGammaPower);
                feature_vector.push_back(features.higherOrderStatistics.thirdMoment);

                // Fill the input tensor with your features
                float* input_data = input_tensor->data.f;
                for (size_t i = 0; i < feature_vector.size(); ++i) {
                    input_data[i] = feature_vector[i];
                }

                // Run inference
                interpreter->Invoke();

                // Use the output
                float* output_data = output_tensor->data.f;
                // Assuming your model outputs probabilities for two classes
                float prob_onehand = output_data[0];
                float prob_twohands = output_data[1];

                std::cout << "Probability of One Hand: " << prob_onehand << std::endl;
                std::cout << "Probability of Two Hands: " << prob_twohands << std::endl;

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