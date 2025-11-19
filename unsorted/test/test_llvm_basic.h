//
// test_llvm_basic.h - Base test class for LLVM tests
//
// Provides common utilities for LLVM testing:
// - Dynamic test naming
// - Workspace management for test isolation
//

#ifndef TEST_LLVM_BASIC_H
#define TEST_LLVM_BASIC_H

#include <gtest/gtest.h>
#include <string>
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

class test_llvm_basic : public testing::Test {
public:
    // TEST_F(test_case, test) - returns "test_case"
    std::string test_case_name()
    {
        ::testing::TestInfo const *const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        if (test_info) {
            return test_info->test_case_name();
        }
        return "test";
    }

    // TEST_F(test_case, test) - returns "test"
    std::string test_name()
    {
        ::testing::TestInfo const *const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        if (test_info) {
            return test_info->name();
        }
        return "test";
    }

    // create a workspace directory in the current directory
    fs::path create_workspace()
    {
        auto workspace_directory = fs::path("out") / test_case_name() / test_name();
        std::error_code ec;
        (void)remove_all(workspace_directory, ec);
        (void)create_directories(workspace_directory, ec);
        if (!fs::is_directory(workspace_directory))
            throw std::runtime_error("Cannot create workspace_directory");
        return workspace_directory;
    }
};

#endif // TEST_LLVM_BASIC_H

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
