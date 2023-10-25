cmake_minimum_required(VERSION 3.0)
project(professor_x)

# Add the source file(s) to your project
add_executable(post_process post_process.cpp)

# Specify the include directories if needed
# include_directories(/path/to/include)

# Specify the library directories
link_directories(/home/pi/Download/fftw-3.3.10)

# Link against the required libraries
target_link_libraries(post_process PRIVATE stdc++ m fftw3)
