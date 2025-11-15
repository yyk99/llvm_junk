//
//
//

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include <vector>
#include <iostream>

using namespace llvm;

static LLVMContext Context;
static std::vector<std::string> FunArgs;

Function *createFunc(IRBuilder<> &Builder, std::string Name, Module *ModuleOb)
{
    Type *u32Ty = Type::getInt32Ty(Context);
    Type *vecTy = FixedVectorType::get(u32Ty, 2);
    Type *ptrTy = PointerType::get(vecTy, 0);
    FunctionType *funcType =
        FunctionType::get(Builder.getInt32Ty(), ptrTy, false);
    Function *fooFunc =
        Function::Create(funcType, Function::ExternalLinkage, Name, ModuleOb);
    return fooFunc;
}

void setFuncArgs(Function *fooFunc, std::vector<std::string> FunArgs)
{
    unsigned Idx = 0;

    Function::arg_iterator AI, AE;
    for (AI = fooFunc->arg_begin(), AE = fooFunc->arg_end(); AI != AE;
         ++AI, ++Idx)
        AI->setName(FunArgs[Idx]);
}

BasicBlock *createBB(Function *fooFunc, std::string Name)
{
    return BasicBlock::Create(Context, Name, fooFunc);
}

Value *getGEP(IRBuilder<> &Builder, Value *Base, Value *Offset)
{
    // With opaque pointers in LLVM 15+, we cannot query the element type from a pointer.
    // The type must be passed explicitly to CreateGEP.
    Type *vecTy = FixedVectorType::get(Builder.getInt32Ty(), 2);
    return Builder.CreateGEP(vecTy, Base, Offset, "a1");
}

Value *getLoad(IRBuilder<> &Builder, Value *Address)
{
    // CreateLoad requires explicit type parameter in LLVM 15+
    Type *i32Ty = Builder.getInt32Ty();
    return Builder.CreateLoad(i32Ty, Address, "load");
}

int main(int argc, char *argv[])
{
    FunArgs.push_back("a");
    IRBuilder<> Builder(Context);
    Module *ModuleOb = new Module("my compiler", Context);

    Function *fooFunc = createFunc(Builder, "foo", ModuleOb);
    setFuncArgs(fooFunc, FunArgs);
    Value *Base = fooFunc->arg_begin();
    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);
    Value *gep = getGEP(Builder, Base, Builder.getInt32(1));

    Value *load = getLoad(Builder, gep);
    Builder.CreateRet(load);
    verifyFunction(*fooFunc);
    ModuleOb->print(outs(), nullptr);

    return 0;
}

// Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM libraries to design a compiler (pp. 42-43). Packt Publishing. Kindle Edition. 


// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:

