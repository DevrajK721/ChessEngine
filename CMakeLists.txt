cmake_minimum_required(VERSION 4.0)
project(ChessEngine)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories for header files
include_directories(include)

# Add main source files from src/
file(GLOB SRC_FILES src/*.cpp)
add_executable(ChessEngine ${SRC_FILES})

# Enable testing and find GTest
include(CTest)
enable_testing()
add_subdirectory(googletest)
add_subdirectory(tests)
