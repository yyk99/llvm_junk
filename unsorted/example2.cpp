//
//
//
//

#include <iostream>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <vector>

using namespace llvm;

static LLVMContext Context;

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

// Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
// libraries to design a compiler (p. 15). Packt Publishing. Kindle Edition.

int main(int argc, char *argv[])
{
    Module *ModuleOb = new Module("my compiler", Context);
    IRBuilder<> Builder(Context);

    Function *fooFunc = createFunc(Builder, "foo", ModuleOb);
    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);
    Builder.CreateRet(Builder.getInt32(0));
    verifyFunction(*fooFunc);
    ModuleOb->print(llvm::outs(), nullptr);

    return 0;
}

//  Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using
//  LLVM libraries to design a compiler (p. 15). Packt Publishing. Kindle Edition.
