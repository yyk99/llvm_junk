#ifndef COMPILERTESTBASE_H
#define COMPILERTESTBASE_H

#include <gtest/gtest.h>

#include "parser.h"
#include "parser_bits.h"
#include "TreeNode.h"
#include "llvm_helper.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"

#include <algorithm>
#include <cstdlib>
#include <stack>
#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

using namespace llvm;

extern LLVMContext TheContext;
extern IRBuilder<> Builder;

class CompilerBase {
public:
    LLVMContext &C;

    Module *TheModule;
    Function *F;

    bool verbose;

    CompilerBase()
        : C(TheContext)
        , verbose {false}
    {
        static bool once;
        if (!once) {
            InitializeNativeTarget();
            InitializeNativeTargetAsmPrinter();

            once = true;
        }
    }

    std::string current_test_name() const
    {
        ::testing::TestInfo const *const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        return test_info->name();
    }

    std::string current_case_name() const
    {
        ::testing::TestInfo const *const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        return test_info->test_case_name();
    }

    // Create a workspace directory in the current directory
    fs::path create_workspace()
    {
        auto workspace_directory = fs::path("out") / current_case_name() / current_test_name();
        std::error_code ec;
        (void)fs::remove_all(workspace_directory, ec);
        (void)fs::create_directories(workspace_directory, ec);
        if (!fs::is_directory(workspace_directory))
            throw std::runtime_error("Cannot create workspace_directory");
        return workspace_directory;
    }
};

class CompilerTestBase : public testing::Test, public CompilerBase {
};

#endif
