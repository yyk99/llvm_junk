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

class test_function_nested : public test_llvm_basic {
public:

    Function *createFunc(IRBuilder<> &Builder, std::string Name, Module *ModuleOb,
                        std::vector<std::string> const &FunArgs)
    {
        std::vector<Type *> Integers(FunArgs.size(), Type::getInt32Ty(Context));
        auto funcType = FunctionType::get(Builder.getInt32Ty(), Integers, false);
        auto fooFunc = Function::Create(funcType, Function::ExternalLinkage, Name, ModuleOb);

        return fooFunc;
    }

    Function *createInternalFunc(IRBuilder<> &Builder, std::string Name, Module *ModuleOb,
                        std::vector<std::string> const &FunArgs)
    {
        std::vector<Type *> Integers(FunArgs.size(), Type::getInt32Ty(Context));
        auto funcType = FunctionType::get(Builder.getInt32Ty(), Integers, false);
        auto fooFunc = Function::Create(funcType, Function::InternalLinkage, Name, ModuleOb);

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

/*
//
// https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/advanced-constructs/lambda-functions.html
//

int foo(int a)
{
  auto function = [a](int x) { return x + a; };
  return function(10);
}

define internal i32 @lambda(i32 %a, i32 %x) {
	%1 = add i32 %a, %x
	ret i32 %1
}

define i32 @foo(i32 %a) {
	%1 = call i32 @lambda(i32 %a, i32 10)
	ret i32 %1
}

*/

TEST_F(test_function_nested, main)
{
    IRBuilder<> Builder(Context);
    auto ModuleOb = std::make_unique<Module>("my compiler", Context);

    auto lambda = createInternalFunc(Builder, "lambda", ModuleOb.get(), {"a", "x"});

    auto fooFunc = createFunc(Builder, "foo", ModuleOb.get(), {"a"});
    {
        setFuncArgs(fooFunc, {"a"});
        BasicBlock *entry = createBB(fooFunc, "entry");
        Builder.SetInsertPoint(entry);

        auto args = lambda->arg_begin();
        Value *a = args++;
        Value *x = Builder.getInt32(10); // Second parameter: 10

        // Call the function using CreateCall
        Value *callResult = Builder.CreateCall(lambda, {a, x});

        Builder.CreateRet(callResult);
        verifyFunction(*fooFunc);
    }
    {
        setFuncArgs(lambda, {"a", "x"});
        BasicBlock *entry = createBB(lambda, "entry");
        Builder.SetInsertPoint(entry);
        // Name the parameters
        auto args = lambda->arg_begin();
        Value *a = args++;
        a->setName("a");
        Value *x = args++;
        x->setName("x");

        Value *result = Builder.CreateAdd(a, x);
        Builder.CreateRet(result);
        verifyFunction(*lambda);
    }
    // Create workspace directory for output
    auto ws = create_workspace();
    auto output_file = ws / "output.ll";
    // Redirect output to file
    std::error_code EC;
    llvm::raw_fd_ostream output(output_file.string(), EC);
    ASSERT_FALSE(EC) << "Failed to open output file: " << EC.message();

    ModuleOb->print(output, nullptr);
#ifndef NDEBUG
    ModuleOb->dump();
#endif

    // Verify file was created and has content
    ASSERT_TRUE(fs::is_regular_file(output_file)) << "Output file was not created";
    EXPECT_LE(0, fs::file_size(output_file)) << "Output file is empty";
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
