# 1) Locate GTest and Threads
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# 2) Build the test executable
add_executable(test_env test_env.cpp)

# 3) Link in GTest and threads
target_link_libraries(test_env
  PRIVATE
    GTest::gtest_main         # this is the static gtest_main target
    Threads::Threads
)

# Include engine sources for the board initialization test
set(ENGINE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/board.cpp
    ${CMAKE_SOURCE_DIR}/src/attacks.cpp
    ${CMAKE_SOURCE_DIR}/src/movegen.cpp
    ${CMAKE_SOURCE_DIR}/src/engine.cpp
    ${CMAKE_SOURCE_DIR}/src/util.cpp
)
add_executable(board_init_test board_init.cpp ${ENGINE_SOURCES})
add_executable(movegen_test movegen_test.cpp ${ENGINE_SOURCES})
add_executable(engine_test engine_test.cpp ${ENGINE_SOURCES})

target_link_libraries(board_init_test
  PRIVATE
    GTest::gtest_main         # this is the static gtest_main target
    Threads::Threads
)

target_link_libraries(movegen_test
  PRIVATE
    GTest::gtest_main         # this is the static gtest_main target
    Threads::Threads
)

target_link_libraries(engine_test
  PRIVATE
    GTest::gtest_main
    Threads::Threads
)

# 4) Register the test with CTest
add_test(
    NAME EnvSanityCheck
    COMMAND test_env
)

add_test(
    NAME BoardInitialization
    COMMAND board_init_test
)

add_test(
    NAME MoveGeneration
    COMMAND movegen_test
)

add_test(
    NAME Engine
    COMMAND engine_test
)