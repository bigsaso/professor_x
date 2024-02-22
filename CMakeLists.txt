cmake_minimum_required(VERSION 3.0)
project(professor_x)

add_subdirectory(
  "/home/pi/Downloads/tflite/tensorflow_src/tensorflow/lite"
  "${CMAKE_CURRENT_BINARY_DIR}/tensorflow-lite" EXCLUDE_FROM_ALL)

# Set C++ standard and compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Add a variable for the executable name
set(EXECUTABLE_NAME "simulate_udp_live_data" CACHE STRING "Name of the executable")

# Add the source file(s) to your project
add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_NAME}.cpp statistical_features.cpp fftw.cpp fftw_relative_powers.cpp shannon_entropy.cpp zero_crossing_rate.cpp hjorth_parameters.cpp stft_relative_powers.cpp higher_order_statistics.cpp features_analysis.cpp)

# Specify the include directories if needed
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Set the base directory for the dependencies
set(DEPS_DIR "/home/pi/Downloads/tflite/tflite_build/_deps")

# Specify the library directories -- Set this up properly
link_directories(/home/pi/Download/fftw-3.3.10)
link_directories(/usr/local/lib)

# Link against the required libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE stdc++ m fftw3 tensorflow-lite)
