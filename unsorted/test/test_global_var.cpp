//
//
//

#include <gtest/gtest.h>

#include <vector>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static LLVMContext Context; // = getGlobalContext();

class test_llvm_basic : public testing::Test {
public:
    // TEST_F(test_case, test) - returns "test_case"
    std::string test_case_name()
    {
        const ::testing::TestInfo *const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        if (test_info) {
            return test_info->test_case_name();
        }
        return "test";
    }

    // TEST_F(test_case, test) - returns "test"
    std::string test_name()
    {
        const ::testing::TestInfo *const test_info =
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

    Function *createFunc(IRBuilder<> &Builder, std::string Name, Module *ModuleOb)
    {
        FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), false);
        Function *fooFunc =
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);
        return fooFunc;
    }

    BasicBlock *createBB(Function *fooFunc, std::string Name)
    {
        return BasicBlock::Create(Context, Name, fooFunc);
    }

    GlobalVariable *createGlob(IRBuilder<> &Builder, std::string Name, Module *ModuleOb)
    {
        ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
        GlobalVariable *gVar = ModuleOb->getNamedGlobal(Name);
        ConstantInt *constIntVal = ConstantInt::get(Builder.getInt32Ty(), 0);
        gVar->setInitializer(constIntVal);
        gVar->setLinkage(GlobalValue::CommonLinkage);
        gVar->setAlignment(MaybeAlign(4));

        return gVar;
    }
};

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

class test_global_var : public test_llvm_basic {
public:
};

//  Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
//  libraries to design a compiler (p. 17). Packt Publishing. Kindle Edition.

TEST_F(test_global_var, main)
{
    IRBuilder<> Builder(Context);
    auto ModuleOb = std::make_unique<Module>("my compiler", Context);

    GlobalVariable *gVar = createGlob(Builder, "x", ModuleOb.get());
    Function *fooFunc = createFunc(Builder, "foo", ModuleOb.get());
    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);
    Builder.CreateRet(Builder.getInt32(0));
    verifyFunction(*fooFunc);

    // Create workspace directory for output
    auto ws = create_workspace();
    auto output_file = ws / "output.ll";
    // Redirect output to file
    std::error_code EC;
    llvm::raw_fd_ostream output(output_file.string(), EC);
    ASSERT_FALSE(EC) << "Failed to open output file: " << EC.message();

    ModuleOb->print(output, nullptr, true, true);
    output.close();

    // Verify file was created and has content
    ASSERT_TRUE(fs::is_regular_file(output_file)) << "Output file was not created";
    EXPECT_LE(0, fs::file_size(output_file)) << "Output file is empty";
}

// Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
// libraries to design a compiler (pp. 17-18). Packt Publishing. Kindle Edition.

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
