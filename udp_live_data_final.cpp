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
                    0.008766941128661169,
                    -0.5163828963053286,
                    18.789033940372978,
                    0.0033610710025781297,
                    0.008430050297967282,
                    0.23371823861148722,
                    0.26764958117377546,
                    0.005923435315469626,
                    1.6774030595225673,
                    813.7751673828361,
                    0.0024626561302796726,
                    0.005634546851066067,
                    0.2785159317877636,
                    0.22742071460022975,
                    -567742.9624711577
                };

                // Std Deviations imported from trained model
                std::vector<float> feature_std_devs = {
                    0.9881721809825954,
                    2.1847848489774786,
                    20.96172885650971,
                    0.013137574838904558,
                    0.020360254299991994,
                    0.0844387963098916,
                    0.094688416889555,
                    0.01647607558470322,
                    0.8712593693717688,
                    1413.2070778075251,
                    0.012542240365146643,
                    0.025527120812980416,
                    0.12871067849253046,
                    0.13776269630725865,
                    4107060.5629486465
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
            // Add a delay to simulate live data at 512Hz
            this_thread::sleep_for(chrono::microseconds(static_cast<int>(delayMilliseconds * 1000.0)));
        }
    }

    return 0;
}