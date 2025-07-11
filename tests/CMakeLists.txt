# 1) Locate GTest and Threads
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# 2) Build the test executables
add_executable(test_env test_env.cpp)

# Include engine sources for the board initialization test
set(ENGINE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/board.cpp
    ${CMAKE_SOURCE_DIR}/src/attacks.cpp
)
add_executable(board_init_test board_init.cpp ${ENGINE_SOURCES})

# 3) Link in GTest and threads
target_link_libraries(test_env
  PRIVATE
    gtest_main
    Threads::Threads
)

target_link_libraries(board_init_test
  PRIVATE
    gtest_main
    Threads::Threads
)

# 4) Register the tests with CTest
add_test(
    NAME EnvSanityCheck
    COMMAND test_env
)

add_test(
    NAME BoardInitialization
    COMMAND board_init_test
)

