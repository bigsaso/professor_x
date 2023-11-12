cmake_minimum_required(VERSION 3.0)
project(professor_x)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Add the source file(s) to your project
add_executable(simulate_live_data simulate_live_data.cpp statistical_features.cpp fftw.cpp fftw_relative_powers.cpp shannon_entropy.cpp zero_crossing_rate.cpp hjorth_parameters.cpp stft_relative_powers.cpp features_analysis.cpp)

# Specify the include directories if needed
target_include_directories(simulate_live_data PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(simulate_live_data PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Specify the library directories
link_directories(/home/pi/Download/fftw-3.3.10)

# Link against the required libraries
target_link_libraries(simulate_live_data PRIVATE stdc++ m fftw3)