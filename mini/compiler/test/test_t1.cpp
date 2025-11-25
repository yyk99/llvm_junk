//
//
//

#include <gtest/gtest.h>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include <filesystem>

namespace fs = std::filesystem;

#include <fstream>
#include <iostream>

using namespace llvm;

class SamplesTestBase  : public ::testing::Test {
public:
    LLVMContext Context;

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

class SamplesF : public SamplesTestBase {};

TEST_F(SamplesF, array_example)
{
    auto ModuleOb = std::make_unique<Module>("array_example", Context);
    IRBuilder<> Builder(Context);

    // Create the main function: int main()
    FunctionType *funcType = FunctionType::get(Builder.getInt32Ty(), false);
    Function *mainFunc =
        Function::Create(funcType, Function::ExternalLinkage, "main", ModuleOb.get());

    // Create the entry basic block
    BasicBlock *entry = BasicBlock::Create(Context, "entry", mainFunc);
    Builder.SetInsertPoint(entry);

    // Create array type: [10 x i32]
    Type *i32Type = Type::getInt32Ty(Context);
    ArrayType *arrayType = ArrayType::get(i32Type, 10);

    // Allocate array on the stack
    AllocaInst *arrayAlloca = Builder.CreateAlloca(arrayType, nullptr, "myArray");

    // Store values into the array
    // Store 42 at index 0
    Value *index0 = Builder.getInt32(0);
    Value *gep0 =
        Builder.CreateGEP(arrayType, arrayAlloca, {Builder.getInt32(0), index0}, "element0");
    Builder.CreateStore(Builder.getInt32(42), gep0);

    // Store 100 at index 5
    Value *index5 = Builder.getInt32(5);
    Value *gep5 =
        Builder.CreateGEP(arrayType, arrayAlloca, {Builder.getInt32(0), index5}, "element5");
    Builder.CreateStore(Builder.getInt32(100), gep5);

    // Store 999 at index 9
    Value *index9 = Builder.getInt32(9);
    Value *gep9 =
        Builder.CreateGEP(arrayType, arrayAlloca, {Builder.getInt32(0), index9}, "element9");
    Builder.CreateStore(Builder.getInt32(999), gep9);

    // Load value from index 5
    Value *loadedValue = Builder.CreateLoad(i32Type, gep5, "loadedValue");

    // Return the loaded value
    Builder.CreateRet(loadedValue);

    // Verify and print the module
    verifyFunction(*mainFunc);
    ModuleOb->print(llvm::outs(), nullptr);
}

/*
int arr[10];
int foo(int k)
{
  return arr[k];
}

int bar(int k)
{
  int arr[10]; // = {1,2,3,4,5,6,7,8,9,0};
  return arr[k];
}

int foobar (int k)
{
    struct {int arr[10]; } s;
    return s.arr[k];
}

; ModuleID = 'arr.c'
source_filename = "arr.c"

%struct.anon = type { [10 x i32] }

@arr = dso_local global [10 x i32] zeroinitializer, align 16

define dso_local i32 @foo(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = sext i32 %3 to i64
  %5 = getelementptr inbounds [10 x i32], ptr @arr, i64 0, i64 %4
  %6 = load i32, ptr %5, align 4
  ret i32 %6
}

define dso_local i32 @bar(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca [10 x i32], align 16
  store i32 %0, ptr %2, align 4
  %4 = load i32, ptr %2, align 4
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds [10 x i32], ptr %3, i64 0, i64 %5
  %7 = load i32, ptr %6, align 4
  ret i32 %7
}

define dso_local i32 @foobar(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca %struct.anon, align 4
  store i32 %0, ptr %2, align 4
  %4 = getelementptr inbounds %struct.anon, ptr %3, i32 0, i32 0
  %5 = load i32, ptr %2, align 4
  %6 = sext i32 %5 to i64
  %7 = getelementptr inbounds [10 x i32], ptr %4, i64 0, i64 %6
  %8 = load i32, ptr %7, align 4
  ret i32 %8
}
*/

TEST_F(SamplesF, array_example_foo)
{
    auto ModuleOb = std::make_unique<Module>(current_test_name(), Context);
    IRBuilder<> Builder(Context);

    {
        // Create a function: int foo()
        FunctionType *funcType = FunctionType::get(Builder.getInt32Ty(), false);
        Function *foo =
            Function::Create(funcType, Function::ExternalLinkage, "foo", ModuleOb.get());
        // Create the entry basic block
        BasicBlock *entry = BasicBlock::Create(Context, "entry", foo);
        Builder.SetInsertPoint(entry);

        // Create array type: [10 x i32]
        Type *i32Type = Type::getInt32Ty(Context);
        ArrayType *array_arr_type = ArrayType::get(i32Type, 10);

        // Allocate array on the stack
        AllocaInst *array_arr = Builder.CreateAlloca(array_arr_type, nullptr, "arr");

        // Return the loaded value
        Builder.CreateRet(Builder.getInt32(123)); // return 123;

        // Verify and print the module
        verifyFunction(*foo);
    }
    ModuleOb->print(llvm::outs(), nullptr);
}

namespace llvm {

class MiniArrayType : public Type {
    uint64_t LowerBound;
    uint64_t NumElements;
    MiniArrayType(Type *ElType, uint64_t NumEl, uint64_t LowBnd);

public:
    MiniArrayType(const MiniArrayType &) = delete;
    MiniArrayType &operator=(const MiniArrayType &) = delete;

    /// This static method is the primary way to construct an ArrayType
    static MiniArrayType *get(Type *ElementType, uint64_t NumElements, uint64_t LowerBound);
};

 MiniArrayType::MiniArrayType(Type *ElType, uint64_t NumEl, uint64_t LowBnd)
    : Type(ElType->getContext(), ArrayTyID)
    , LowerBound(LowBnd)
    , NumElements(NumEl)
 {
 }

MiniArrayType *MiniArrayType::get(Type *ElementType, uint64_t NumElements,
                                        uint64_t LowerBound)
{
    return new MiniArrayType(ElementType, NumElements, LowerBound);
}

}

/// @brief Use custom array definition
/// @param --gtest_filter=SamplesF.array_mini_example
/// @param  
TEST_F(SamplesF, array_mini_example)
{
    auto ModuleOb = std::make_unique<Module>(current_test_name(), Context);
    IRBuilder<> Builder(Context);

    // Create a function: int foo()
    FunctionType *funcType = FunctionType::get(Builder.getInt32Ty(), false);
    Function *foo = Function::Create(funcType, Function::ExternalLinkage, "foo", ModuleOb.get());
    // Create the entry basic block
    BasicBlock *entry = BasicBlock::Create(Context, "entry", foo);
    Builder.SetInsertPoint(entry);

    // Create array type: [10 x i32]
    Type *i32Type = Type::getInt32Ty(Context);
    MiniArrayType *arrayType = MiniArrayType::get(i32Type, 10, 1);

    // Allocate array on the stack
    AllocaInst *arrayAlloca = Builder.CreateAlloca(arrayType, nullptr, "mini_array");

    // Return the loaded value
    Builder.CreateRet(Builder.getInt32(123)); // return 123;

    // Verify and print the module
    verifyFunction(*foo);
    ModuleOb->print(llvm::outs(), nullptr);
}
// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
