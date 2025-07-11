cmake_minimum_required(VERSION 3.16)
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

# Enable testing and GoogleTest
include(CTest)
enable_testing()

# Fetch GoogleTest at configure time
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)
