# 1) Locate GTest and Threads
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# 2) Build the test executable
add_executable(test_env test_env.cpp)

# 3) Link in GTest and threads
target_link_libraries(test_env
  PRIVATE
    gtest_main         # this is the static gtest_main target
    Threads::Threads
)

# 4) Register the test with CTest
add_test(
    NAME EnvSanityCheck
    COMMAND test_env
)

