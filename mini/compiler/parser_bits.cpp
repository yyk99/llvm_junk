//
//
//

#include "parser_bits.h"
#include "TreeNode.h"

// #include <iostream>

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
//#include <cctype>
//#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

void init_compiler()
{
    // TODO: place init code here
}

//
// process main programm
//
void program_header(TreeNode *node)
{
    auto id = dynamic_cast<TreeIdentNode *>(node);

    TheModule = llvm::make_unique<llvm::Module>(id->id, TheContext);

    std::vector<llvm::Type *> Doubles(0, llvm::Type::getDoubleTy(TheContext));
    llvm::FunctionType *FT =
        llvm::FunctionType::get(Builder.getInt32Ty(), Doubles, false);

    llvm::Function *F =
        llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", TheModule.get());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName("arg");

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);

    auto rc = Builder.getInt32(0);

    Builder.CreateRet(rc);

    verifyFunction(*F);
}

void program_end(TreeNode *node)
{
    auto id = dynamic_cast<TreeIdentNode *>(node);
    // TODO: verify ending label == module name
    TheModule->print(llvm::outs(), nullptr);
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
