cmake_minimum_required(VERSION 3.10)
project(CrowServer)

set(CMAKE_CXX_STANDARD 17)

# Add Crow include directory
include_directories(${Server}/external/Crow)

# Add your source files
add_executable(main src/main.cpp src/VmManager.cpp)

# Link necessary libraries
target_link_libraries(main pthread)
