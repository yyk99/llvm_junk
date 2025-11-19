//
// Tests for test_llvm_basic utility class
//

#include "test_llvm_basic.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// Test the test_llvm_basic helper methods
TEST_F(test_llvm_basic, test_case_name)
{
    EXPECT_EQ("test_llvm_basic", test_case_name());
}

TEST_F(test_llvm_basic, test_name)
{
    EXPECT_EQ("test_name", test_name());
}

TEST_F(test_llvm_basic, create_workspace)
{
    auto ws = create_workspace();
    ASSERT_TRUE(fs::is_directory(ws));

    std::ofstream s(ws / "test.txt");
    ASSERT_TRUE(s.good());
    s.close();
    EXPECT_TRUE(fs::is_regular_file(ws / "test.txt"));
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
