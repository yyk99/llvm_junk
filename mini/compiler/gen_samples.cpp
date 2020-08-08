#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;


int
main()
{
    TheModule = llvm::make_unique<llvm::Module>("my cool jit", TheContext);

    std::vector<llvm::Type *> Doubles(0, llvm::Type::getDoubleTy(TheContext));
    llvm::FunctionType *FT =                                                     
          llvm::FunctionType::get(llvm::Type::getDoubleTy(TheContext), Doubles, false);
                                                                          
    llvm::Function *F =
          llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "__anon__", TheModule.get());  
                                                                                  
    // Set names for all arguments.                                              
    unsigned Idx = 0;                                                           
    for (auto &Arg : F->args())
          Arg.setName("arg");
                                                                             
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);

    llvm::Value *L = llvm::ConstantFP::get(TheContext, llvm::APFloat(42.0));
    llvm::Value *R = llvm::ConstantFP::get(TheContext, llvm::APFloat(11.0));
    auto value3 = Builder.CreateFAdd(L, R, "addtmp");


    Builder.CreateRet(value3);

    verifyFunction(*F);


    TheModule->print(llvm::errs(), nullptr);
    
    printf("Hello...\n");
}
