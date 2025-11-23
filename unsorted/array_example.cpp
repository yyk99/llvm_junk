//
// Example: Creating and using an array of 10 integers in LLVM IR
//

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

int main()
{
    LLVMContext Context;
    Module *ModuleOb = new Module("array_example", Context);
    IRBuilder<> Builder(Context);

    // Create the main function: int main()
    FunctionType *funcType = FunctionType::get(Builder.getInt32Ty(), false);
    Function *mainFunc = Function::Create(funcType, Function::ExternalLinkage, "main", ModuleOb);

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
