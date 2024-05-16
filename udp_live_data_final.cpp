#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <wiringPi.h>

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

void standardizeFeatures(std::vector<float>& features, 
                         const std::vector<float>& means, 
                         const std::vector<float>& stdDevs) {
    for (size_t i = 0; i < features.size(); ++i) {
        features[i] = (features[i] - means[i]) / stdDevs[i];
    }
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

    // Initialize GPIO pins
    wiringPiSetup();			// Setup the library
    int forwardPin = 0;
    int backwardPin = 1;
    int leftPin = 2;
    int rightPin = 3;
    pinMode(forwardPin, OUTPUT);		// Configure GPIO0 as an output
    pinMode(backwardPin, OUTPUT);		// Configure GPIO1 as an output
    pinMode(leftPin, OUTPUT);		// Configure GPIO2 as an output
    pinMode(rightPin, OUTPUT);		// Configure GPIO3 as an output

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

                // Means imported from trained model
                std::vector<float> feature_means = {
                    -0.006544112651068523,
                    -0.5463788923961688,
                    18.93126237872342,
                    0.003165531318788307,
                    0.008200745927063803,
                    0.23389299444680822,
                    0.2678451070000005,
                    0.0057141183460851265,
                    1.6718270217021205,
                    811.2059479812781,
                    0.0022576864175801177,
                    0.005246508891954272,
                    0.27863311479574454,
                    0.22759236279148862,
                    -562735.5761630366
                };

                // Std Deviations imported from trained model
                std::vector<float> feature_std_devs = {
                    0.9805617111691372,
                    2.1672786378470192,
                    21.025074577473422,
                    0.012556485752110123,
                    0.019550251192301353,
                    0.0849010459867427,
                    0.0944644005573779,
                    0.015787734144267446,
                    0.873865813438969,
                    1403.6032115007679,
                    0.01191603022225892,
                    0.024263757885246196,
                    0.12945193080239317,
                    0.1380254247877992,
                    4257149.463629578
                };

                // Standardize the feature vector
                standardizeFeatures(feature_vector, feature_means, feature_std_devs);

                // Fill the input tensor with your features
                float* input_data = input_tensor->data.f;
                for (size_t i = 0; i < feature_vector.size(); ++i) {
                    input_data[i] = feature_vector[i];
                }

                // Run inference
                interpreter->Invoke();

                // Use the output
                float* output_data = output_tensor->data.f;
                // Model outputs 4 probabilities
                float prob_forward = output_data[0];
                float prob_backward = output_data[1];
                float prob_left = output_data[2];
                float prob_right = output_data[3];

                std::cout << "Probability of going Forward: " << prob_forward << std::endl;
                std::cout << "Probability of going Backward: " << prob_backward << std::endl;
                std::cout << "Probability of going Left: " << prob_left << std::endl;
                std::cout << "Probability of going Right: " << prob_right << std::endl;

                if(prob_forward > 0.85){
                    digitalWrite(forwardPin, 1);
                    digitalWrite(backwardPin, 0);
                    digitalWrite(leftPin, 0);
                    digitalWrite(rightPin, 0);
                }
                else if(prob_backward > 0.85){
                    digitalWrite(forwardPin, 0);
                    digitalWrite(backwardPin, 1);
                    digitalWrite(leftPin, 0);
                    digitalWrite(rightPin, 0);
                }
                else if(prob_left > 0.85){
                    digitalWrite(forwardPin, 0);
                    digitalWrite(backwardPin, 0);
                    digitalWrite(leftPin, 1);
                    digitalWrite(rightPin, 0);
                }
                else if(prob_right > 0.85){
                    digitalWrite(forwardPin, 0);
                    digitalWrite(backwardPin, 0);
                    digitalWrite(leftPin, 0);
                    digitalWrite(rightPin, 1);
                }

                // Then clear the buffer
                buffer.clear();
                // And increment the counter
                ++counter;
            }
            // // Add a delay to simulate live data at 512Hz -- Removed for live data deployment
            // this_thread::sleep_for(chrono::microseconds(static_cast<int>(delayMilliseconds * 1000.0)));
        }
    }

    return 0;
}