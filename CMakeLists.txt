cmake_minimum_required(VERSION 3.0)
project(professor_x)

# Add the source file(s) to your project
#add_executable(post_process post_process.cpp)
#add_executable(process process.cpp)
add_executable(simulate_live_data simulate_live_data.cpp)

# Specify the include directories if needed
# include_directories(/path/to/include)
# find_package( OpenCV REQUIRED )
# include_directories( ${OpenCV_INCLUDE_DIRS} )

# Specify the library directories
link_directories(/home/pi/Download/fftw-3.3.10)

# Link against the required libraries
# target_link_libraries(post_process PRIVATE ${OpenCV_LIBS} stdc++ m fftw3)
#target_link_libraries(process PRIVATE stdc++ m fftw3)
target_link_libraries(simulate_live_data PRIVATE stdc++ m fftw3)