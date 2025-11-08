/*

This file is a sample/test program demonstrating LLVM IR generation using the LLVM C++ API. Here's my analysis:

  Purpose

  - Generates sample LLVM IR code to demonstrate various LLVM API features
  - Creates two modules (module001 and module002) with example functions
  - Shows how to work with types, allocations, structs, arrays, and function calls

*/

#define NDEBUG

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
//#include "llvm/IR/Type.h"
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
#include <stack>

#include "llvm_helper.h"

using namespace llvm;

static llvm::LLVMContext C;
static llvm::IRBuilder<> Builder(C);

std::stack<Module *> modules;

static llvm::Module * TheModule()
{
    return modules.top();
}


GlobalVariable *create_global_array(IRBuilder<> &Builder, std::string Name, int size)
{
    auto type = Builder.getInt32Ty();
    TheModule()->getOrInsertGlobal(Name, type);

    GlobalVariable *gVar = TheModule()->getNamedGlobal(Name);

    gVar->setLinkage(GlobalValue::CommonLinkage);
    gVar->setAlignment(MaybeAlign(4));

    // Constant Definitions
    auto zero = ConstantInt::get(Type::getInt32Ty(C), 0);

    // Global Variable Definitions
    gVar->setInitializer(zero);

    return gVar;
}

typedef ArrayRef<Type *> TypeArray;

Type *arrayPassport[] = {
    Builder.getInt32Ty(),    // lower boundary, 1 is default
    Builder.getInt32Ty(),    // upper boundary
    PointerType::getUnqual(Type::getInt32Ty(C)),  // data
    Builder.getInt32Ty(),    // reserved
};

Type *CreateStructType ()
{
    return StructType::get(C, TypeArray(arrayPassport));
}

Type *arrayPassport2[] = {
    PointerType::getUnqual(Type::getInt32Ty(C)),  // data
    Builder.getInt32Ty(),    // reserved
};

Type *CreateStructType2 ()
{
    return StructType::get(C, TypeArray(arrayPassport2));
}

Type *CreateStructType3()
{
    std::vector<Type *> items{Type::getInt32Ty(C), Type::getFloatTy(C)};
    Type *structType = StructType::get(C, TypeArray(items));

    return structType;
}

int
main(int argc, char **argv)
{
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    {
        modules.push(new llvm::Module("module001", C));
        Module *M = TheModule();

        auto glob1 = create_global_array(Builder, "glob1", 10);

        std::vector<llvm::Type *> formal_args(2, llvm::Type::getInt32Ty(C));
        FunctionType *FT = FunctionType::get(llvm::Type::getInt32Ty(C), formal_args, false);
        Function *F = Function::Create(FT, llvm::Function::ExternalLinkage, "foo", TheModule());  
                                                                                  
        // Set names for all arguments.                                              
        unsigned Idx = 0;                  
        for (auto &Arg : F->args())
            Arg.setName("arg");
                                                                             
        BasicBlock *BB = llvm::BasicBlock::Create(C, "entry", F);
        Builder.SetInsertPoint(BB);

        {
            // allocate string?
            // NOTE: Builder must have BB set
            auto String1a = Builder.CreateGlobalStringPtr("TestString", "String1a");
        }
    
        auto _1 = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "loc");
        auto _2 = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "loc");
        auto _3 = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "loc");

        // allocate memory for struct type
        Type *struct_type = CreateStructType();
        auto _5 = Builder.CreateAlloca(struct_type, 0, "pass");

        // load address of the struct
        //  Value *CreateStructGEP(Type *Ty, Value *Ptr, unsigned Idx, const Twine &Name = "")
        {
            auto _6 = Builder.CreateStructGEP(struct_type, _5, 0);
            Value *c42 = Builder.getInt32(42);
            auto _7 = Builder.CreateStore(c42, _6);
        }
        {
            auto _6 = Builder.CreateStructGEP(struct_type, _5, 1);
            Value *c42 = Builder.getInt32(43);
            auto _7 = Builder.CreateStore(c42, _6);
        }
        {
            auto _6 = Builder.CreateStructGEP(struct_type, _5, 2);

            std::vector<llvm::Type *> formal_args(1, Type::getInt32Ty(C));
            FunctionType *FT = FunctionType::get(PointerType::getUnqual(Type::getInt32Ty(C)), formal_args, false);
            Function *F = Function::Create(FT, llvm::Function::ExternalLinkage, "allocate_array", TheModule());

            Value *c42 = Builder.getInt32(123);
            auto call_F = Builder.CreateCall(F, {c42});
            auto _7 = Builder.CreateStore(call_F, _6);
        }
        {
            auto _6 = Builder.CreateStructGEP(struct_type, _5, 3);
            Value *c42 = Builder.getInt32(-1);
            auto _7 = Builder.CreateStore(c42, _6);
        }
        {
            Type *u32Ty = Type::getInt32Ty(C);
            Type *vecTy = ArrayType::get(u32Ty, 10);

            auto _4 = Builder.CreateAlloca(vecTy, 0, "arr");
            Value *e1 = ConstantInt::get(Type::getInt32Ty(C), 5);
            Value *zero = Builder.getInt32(0);
            Value *v_pos = Builder.CreateGEP(vecTy, _4, {zero, zero}, "arr_e1");
            Builder.CreateRet(Builder.CreateLoad(u32Ty, v_pos));
        }
        
        Value *L = ConstantInt::get(Type::getInt32Ty(C), 42);
        Value *R = ConstantInt::get(Type::getInt32Ty(C), 12);
        Value *RET = Builder.CreateAdd(L, R, "addtmp");

        {
            std::vector<Type *> items{Type::getInt32Ty(C), Type::getDoubleTy(C)};
            Type *structType = StructType::get(C, TypeArray(items));

            auto _1 = Builder.CreateAlloca(structType, 0, "array");

            show_type_details(_1->getType());
        }
        
        Builder.CreateRet(RET);

        verifyFunction(*F);
    }
    {
        modules.push(new Module("module002", C));
        Module *M = TheModule();

        auto glob1 = create_global_array(Builder, "glob2", 10);

        std::vector<llvm::Type *> formal_args(2, Type::getInt32Ty(C));
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(C), formal_args, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, "bar", TheModule());  
        
        // Set names for all arguments.                                              
        unsigned Idx = 0;                  
        for (auto &Arg : F->args())
            Arg.setName("arg");
        
        BasicBlock *BB = llvm::BasicBlock::Create(C, "entry", F);
        Builder.SetInsertPoint(BB);

        {
            // allocate string?
            // NOTE: Builder must have BB set
            auto String1a = Builder.CreateGlobalStringPtr("TestString2", "String2a");
        }
    
        auto _1 = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "loc");
        auto _2 = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "loc");
        auto _3 = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "loc");

        Value *L = ConstantInt::get(Type::getInt32Ty(C), 42);
        Value *R = ConstantInt::get(Type::getInt32Ty(C), 12);
        Value *RET = Builder.CreateAdd(L, R, "addtmp");
    
        Builder.CreateRet(RET);

        verifyFunction(*F);
    }
    
    TheModule()->print(llvm::outs(), nullptr);
    modules.pop();
    TheModule()->print(llvm::outs(), nullptr);
    
#if 0
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
    outs() << ";; Result: " << GV.IntVal << "\n";
#endif
    return 0;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
