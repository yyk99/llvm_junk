#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace llvm;

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

int
main(int argc, char **argv)
{
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    
    TheModule = llvm::make_unique<llvm::Module>("the_module", TheContext);
    Module *M = TheModule.get();

    std::vector<llvm::Type *> formal_args(2, llvm::Type::getInt32Ty(TheContext));
    llvm::FunctionType *FT =                                                     
          llvm::FunctionType::get(llvm::Type::getInt32Ty(TheContext), formal_args, false);
                                                                          
    llvm::Function *F =
          llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "foo", TheModule.get());  
                                                                                  
    // Set names for all arguments.                                              
    unsigned Idx = 0;                  
    for (auto &Arg : F->args())
          Arg.setName("arg");
                                                                             
    BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);

    auto _1 = Builder.CreateAlloca(llvm::Type::getInt32Ty(TheContext), 0, "loc");
    auto _2 = Builder.CreateAlloca(llvm::Type::getInt32Ty(TheContext), 0, "loc");
    auto _3 = Builder.CreateAlloca(llvm::Type::getInt32Ty(TheContext), 0, "loc");

    Value *L = ConstantInt::get(Type::getInt32Ty(TheContext), 42);
    //    L = Builder.CreateStore(L, _1);
    //    L = Builder.CreateLoad(_1);

    Value *R = ConstantInt::get(Type::getInt32Ty(TheContext), 12);
    Value *RET = Builder.CreateAdd(L, R, "addtmp");
    
    Builder.CreateRet(RET);

    verifyFunction(*F);

    TheModule->print(llvm::outs(), nullptr);

    std::string errStr;
    ExecutionEngine *EE = EngineBuilder(std::move(TheModule))
      .setErrorStr(&errStr)
      .create();

    if (!EE) {
      errs() << argv[0] << ": Failed to construct ExecutionEngine: " << errStr << "\n";
      return 1;
    }

    // Call the Fibonacci function with argument n:
    std::vector<GenericValue> Args(0);
    //    Args[0].IntVal = APInt(32, 123); 
    GenericValue GV = EE->runFunction(F, Args);

    // import result of execution
    outs() << "Result: " << GV.IntVal << "\n";

    return 0;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
