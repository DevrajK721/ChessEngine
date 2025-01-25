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
add_executable(ChessEngine ${SOURCES}
        src/bitboard.cpp
        src/move.cpp
        src/bitboard_display.cpp
        src/bitboard_pawn.cpp
        src/bitboard_knight.cpp
        src/bitboard_sliding.cpp
        src/bitboard_king.cpp
        src/bitboard_misc.cpp 
        src/evaluation.cpp 
        src/search.cpp
        src/zobrist.cpp
        src/main.cpp)

