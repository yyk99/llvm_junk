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
//
https://mapping-high-level-constructs-to-llvm-ir.readthedocs.io/en/latest/advanced-constructs/lambda-functions.html
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

TEST_F(test_function_nested, lambda_c)
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


/*

extern "C" int foo(int a)
{                                                                                                                                  
  auto function = [a](int x) { return x + a; };
  return function(10);
}

; ModuleID = 'lambda.cc'
source_filename = "lambda.cc"

%class.anon = type { i32 }

; Function Attrs: mustprogress noinline optnone uwtable
define dso_local i32 @foo(i32 noundef %0) {
  %2 = alloca i32, align 4
  %3 = alloca %class.anon, align 4
  store i32 %0, ptr %2, align 4
  %4 = getelementptr inbounds %class.anon, ptr %3, i32 0, i32 0
  %5 = load i32, ptr %2, align 4
  store i32 %5, ptr %4, align 4
  %6 = call noundef i32 @"_ZZ3fooENK3$_0clEi"(ptr noundef nonnull align 4 dereferenceable(4) %3, i32 noundef 10)
  ret i32 %6
}

; Function Attrs: mustprogress noinline nounwind optnone uwtable
define internal noundef i32 @"_ZZ3fooENK3$_0clEi"(ptr noundef nonnull align 4 dereferenceable(4) %0, i32 noundef %1) {
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  store ptr %0, ptr %3, align 8
  store i32 %1, ptr %4, align 4
  %5 = load ptr, ptr %3, align 8
  %6 = load i32, ptr %4, align 4
  %7 = getelementptr inbounds %class.anon, ptr %5, i32 0, i32 0
  %8 = load i32, ptr %7, align 4
  %9 = add nsw i32 %6, %8
  ret i32 %9
}
  
*/

TEST_F(test_function_nested, lambda_cpp)
{
    IRBuilder<> Builder(Context);
    auto ModuleOb = std::make_unique<Module>("lambda.cc", Context);

    // Define the closure type: { i32 } - captures variable 'a'
    StructType *ClosureType = StructType::create(Context, "class.anon");
    ClosureType->setBody({Type::getInt32Ty(Context)});

    // Create the internal lambda function (operator())
    // Takes: ptr to closure, i32 parameter -> returns i32
    std::vector<Type *> LambdaParams = {
        PointerType::getUnqual(ClosureType), // this pointer (closure)
        Type::getInt32Ty(Context) // parameter
    };
    FunctionType *LambdaFT = FunctionType::get(Type::getInt32Ty(Context), LambdaParams, false);
    Function *LambdaFunc =
        Function::Create(LambdaFT, Function::InternalLinkage, "_ZZ3fooENK3$_0clEi", ModuleOb.get());

    // Create the foo function
    std::vector<Type *> FooParams = {Type::getInt32Ty(Context)};
    FunctionType *FooFT = FunctionType::get(Type::getInt32Ty(Context), FooParams, false);
    Function *FooFunc = Function::Create(FooFT, Function::ExternalLinkage, "foo", ModuleOb.get());

    // Implement foo function
    {
        BasicBlock *BB = BasicBlock::Create(Context, "", FooFunc);
        Builder.SetInsertPoint(BB);

        // Allocate storage for parameter
        AllocaInst *ParamAlloca = Builder.CreateAlloca(Type::getInt32Ty(Context), nullptr);

        // Allocate closure
        AllocaInst *ClosureAlloca = Builder.CreateAlloca(ClosureType, nullptr);

        // Store parameter
        Value *Param = FooFunc->arg_begin();
        Builder.CreateStore(Param, ParamAlloca);

        // Get pointer to closure member and store captured value
        Value *ClosureMember = Builder.CreateStructGEP(ClosureType, ClosureAlloca, 0);
        Value *LoadedParam = Builder.CreateLoad(Type::getInt32Ty(Context), ParamAlloca);
        Builder.CreateStore(LoadedParam, ClosureMember);

        // Call lambda with closure and constant 10
        Value *CallResult = Builder.CreateCall(LambdaFunc, {ClosureAlloca, Builder.getInt32(10)});

        Builder.CreateRet(CallResult);
        verifyFunction(*FooFunc);
    }

    // Implement lambda function
    {
        BasicBlock *BB = BasicBlock::Create(Context, "", LambdaFunc);
        Builder.SetInsertPoint(BB);

        // Allocate storage for parameters
        AllocaInst *ClosurePtrAlloca =
            Builder.CreateAlloca(PointerType::getUnqual(ClosureType), nullptr);
        AllocaInst *ParamAlloca = Builder.CreateAlloca(Type::getInt32Ty(Context), nullptr);

        // Store parameters
        auto ArgIt = LambdaFunc->arg_begin();
        Value *ClosurePtr = ArgIt++;
        Value *Param = ArgIt;
        Builder.CreateStore(ClosurePtr, ClosurePtrAlloca);
        Builder.CreateStore(Param, ParamAlloca);

        // Load closure pointer
        Value *LoadedClosurePtr =
            Builder.CreateLoad(PointerType::getUnqual(ClosureType), ClosurePtrAlloca);

        // Load parameter
        Value *LoadedParam = Builder.CreateLoad(Type::getInt32Ty(Context), ParamAlloca);

        // Get captured value from closure
        Value *ClosureMember = Builder.CreateStructGEP(ClosureType, LoadedClosurePtr, 0);
        Value *CapturedValue = Builder.CreateLoad(Type::getInt32Ty(Context), ClosureMember);

        // Add parameter + captured value
        Value *Result = Builder.CreateAdd(LoadedParam, CapturedValue, "", false, true); // nsw flag

        Builder.CreateRet(Result);
        verifyFunction(*LambdaFunc);
    }

    // Create workspace directory for output
    auto ws = create_workspace();
    auto output_file = ws / "output.ll";

    // Redirect output to file
    std::error_code EC;
    llvm::raw_fd_ostream output(output_file.string(), EC);
    ASSERT_FALSE(EC) << "Failed to open output file: " << EC.message();

    ModuleOb->print(output, nullptr);
    output.close();

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
