#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include <vector>

using namespace llvm;

static LLVMContext Context; // = getGlobalContext();

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
//  Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
//  libraries to design a compiler (p. 17). Packt Publishing. Kindle Edition.

int main(int argc, char *argv[])
{
    IRBuilder<> Builder(Context);
    Module *ModuleOb = new Module("my compiler", Context);


    GlobalVariable *gVar = createGlob(Builder, "x", ModuleOb);
    Function *fooFunc = createFunc(Builder, "foo", ModuleOb);
    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);
    Builder.CreateRet(Builder.getInt32(0));
    verifyFunction(*fooFunc);

    ModuleOb->print(llvm::outs(), nullptr);

    return 0;
}

// Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
// libraries to design a compiler (pp. 17-18). Packt Publishing. Kindle Edition.
