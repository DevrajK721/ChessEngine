#include <gtest/gtest.h>

// A super‐simple smoke test to verify your build & runtime environment
TEST(EnvironmentTest, SaneArithmetic) {
    EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}