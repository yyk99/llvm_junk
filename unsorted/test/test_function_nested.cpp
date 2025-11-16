//
// Test for nested function generation
// Based on: Sarda, Suyog. LLVM Essentials (pp. 20-21). Packt Publishing.
//

#include "test_llvm_basic.h"

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

static LLVMContext Context;

class test_function_with_args : public test_llvm_basic {
public:

    Function *createFunc(IRBuilder<> &Builder, std::string Name, Module *ModuleOb,
                        std::vector<std::string> const &FunArgs)
    {
        std::vector<Type *> Integers(FunArgs.size(), Type::getInt32Ty(Context));
        FunctionType *funcType = llvm::FunctionType::get(Builder.getInt32Ty(), Integers, false);
        Function *fooFunc =
            llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, Name, ModuleOb);

        return fooFunc;
    }

    void setFuncArgs(Function *fooFunc, std::vector<std::string> const &FunArgs)
    {
        unsigned Idx = 0;
        Function::arg_iterator AI, AE;
        for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE; ++AI, ++Idx)
            AI->setName(FunArgs[Idx]);
    }

    BasicBlock *createBB(Function *fooFunc, std::string Name)
    {
        return BasicBlock::Create(Context, Name, fooFunc);
    }

    GlobalVariable *createGlob(IRBuilder<> &Builder, std::string Name, Module *ModuleOb)
    {
        ModuleOb->getOrInsertGlobal(Name, Builder.getInt32Ty());
        GlobalVariable *gVar = ModuleOb->getNamedGlobal(Name);
        gVar->setInitializer(ConstantInt::get(Builder.getInt32Ty(), 0));
        gVar->setLinkage(GlobalValue::CommonLinkage);
        gVar->setAlignment(MaybeAlign(4));
        return gVar;
    }
};

//  Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
//  libraries to design a compiler (pp. 20-21). Packt Publishing. Kindle Edition.

TEST_F(test_function_with_args, main)
{
    std::vector<std::string> FunArgs;
    FunArgs.push_back("a");
    FunArgs.push_back("b");

    IRBuilder<> Builder(Context);
    auto ModuleOb = std::make_unique<Module>("my compiler", Context);

    GlobalVariable *gVar = createGlob(Builder, "x", ModuleOb.get());
    Function *fooFunc = createFunc(Builder, "foo", ModuleOb.get(), FunArgs);
    setFuncArgs(fooFunc, FunArgs);
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

    ModuleOb->print(output, nullptr);
    output.close();

    // Verify file was created and has content
    ASSERT_TRUE(fs::is_regular_file(output_file)) << "Output file was not created";
    EXPECT_LE(0, fs::file_size(output_file)) << "Output file is empty";

    // Verify function has correct number of arguments
    EXPECT_EQ(fooFunc->arg_size(), 2) << "Function should have 2 arguments";

    // Verify argument names
    auto arg_it = fooFunc->arg_begin();
    EXPECT_EQ(arg_it->getName(), "a") << "First argument should be named 'a'";
    ++arg_it;
    EXPECT_EQ(arg_it->getName(), "b") << "Second argument should be named 'b'";
}

// Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
// libraries to design a compiler (pp. 21-22). Packt Publishing. Kindle Edition.

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
