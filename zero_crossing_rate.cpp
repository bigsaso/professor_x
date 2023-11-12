// zero_crossing_rate.cpp
#include "zero_crossing_rate.h"

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