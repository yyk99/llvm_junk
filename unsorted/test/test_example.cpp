#include <gtest/gtest.h>
#include "llvm/ADT/SmallVector.h"

// Example test demonstrating Google Test with LLVM SmallVector

TEST(SmallVectorTest, BasicOperations) {
    llvm::SmallVector<int, 4> vec;

    // Test push_back
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);

    EXPECT_EQ(vec.size(), 3);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);
}

TEST(SmallVectorTest, Resize) {
    llvm::SmallVector<int, 4> vec;

    vec.resize(5, 42);

    EXPECT_EQ(vec.size(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(vec[i], 42);
    }
}

TEST(SmallVectorTest, InitializerList) {
    llvm::SmallVector<std::string, 3> names{"Alice", "Bob", "Charlie"};

    ASSERT_EQ(names.size(), 3);
    EXPECT_EQ(names[0], "Alice");
    EXPECT_EQ(names[1], "Bob");
    EXPECT_EQ(names[2], "Charlie");
}

// Main is provided by GTest::gtest_main linked in CMakeLists.txt
