cmake_minimum_required(VERSION 3.16)

# Project name and version
project(ChessEngine VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the executable
add_executable(ChessEngine ${SOURCES})

# Enable testing
enable_testing()
add_subdirectory(tests)
