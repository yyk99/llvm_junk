//
// Example: Creating and using an array of 10 integers in LLVM IR
//

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

/*
double foo (int i, int k)
{
    double a[2][3];

    return a[i][k];
}

; ModuleID = 'arr2x3.c'
source_filename = "arr2x3.c"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local double @foo(i32 noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca [2 x [3 x double]], align 16
  store i32 %0, ptr %3, align 4
  store i32 %1, ptr %4, align 4
  %6 = load i32, ptr %3, align 4
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds [2 x [3 x double]], ptr %5, i64 0, i64 %7
  %9 = load i32, ptr %4, align 4
  %10 = sext i32 %9 to i64
  %11 = getelementptr inbounds [3 x double], ptr %8, i64 0, i64 %10
  %12 = load double, ptr %11, align 8
  ret double %12
}

*/


using namespace llvm;

int main()
{
    LLVMContext Context;
    Module *ModuleOb = new Module("array_2d_example", Context);
    IRBuilder<> Builder(Context);

    // Create the function: double foo(int i, int k)
    std::vector<Type *> args_ik(2, Builder.getInt32Ty());
    FunctionType *funcType = FunctionType::get(Builder.getDoubleTy(), args_ik, false);
    Function *mainFunc = Function::Create(funcType, Function::ExternalLinkage, "foo", ModuleOb);

    // Retrieve arguments and proceed with further usage...
    auto args = mainFunc->arg_begin();
    Value *arg_i = &(*args++);
    Value *arg_k = &(*args++);

    arg_i->dump();
    arg_k->dump();

    // Create the entry basic block
    BasicBlock *entry = BasicBlock::Create(Context, "entry", mainFunc);
    Builder.SetInsertPoint(entry);

    // Create array type: [3 x double]
    ArrayType *arrayInnerType = ArrayType::get(Type::getDoubleTy(Context), 3);
    ArrayType *arrayType = ArrayType::get(arrayInnerType, 2);

    // Allocate array on the stack
    AllocaInst *arrayAlloca = Builder.CreateAlloca(arrayType, nullptr, "a");
#if 0
    // Store values into the array
    // Store 42 at index 0
    Value *index0 = Builder.getInt32(0);
    Value *gep0 = Builder.CreateGEP(arrayType, arrayAlloca, {Builder.getInt32(0), index0}, "element0");
    Builder.CreateStore(Builder.getInt32(42), gep0);

    // Store 100 at index 5
    Value *index5 = Builder.getInt32(5);
    Value *gep5 = Builder.CreateGEP(arrayType, arrayAlloca, {Builder.getInt32(0), index5}, "element5");
    Builder.CreateStore(Builder.getInt32(100), gep5);

    // Store 999 at index 9
    Value *index9 = Builder.getInt32(9);
    Value *gep9 = Builder.CreateGEP(arrayType, arrayAlloca, {Builder.getInt32(0), index9}, "element9");
    Builder.CreateStore(Builder.getInt32(999), gep9);

    // Load value from index 5
    Value *loadedValue = Builder.CreateLoad(i32Type, gep5, "loadedValue");
#else
    Value *gep_i =
        Builder.CreateGEP(arrayType, arrayAlloca, {Builder.getInt32(1), arg_i}, "element0");
    Value *gep_k = Builder.CreateGEP(arrayInnerType, gep_i, {Builder.getInt32(1), arg_k}, "element1");
    auto loadedValue = Builder.CreateLoad(Type::getDoubleTy(Context), gep_k);
#endif
    // Return the loaded value
    Builder.CreateRet(loadedValue);

    // Verify and print the module
    verifyFunction(*mainFunc);
    ModuleOb->print(llvm::outs(), nullptr);

    delete ModuleOb;
    return 0;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
