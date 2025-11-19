//
//
//

//
//
//


#include <vector>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include "test_llvm_basic.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static LLVMContext Context;

class test_global_var : public test_llvm_basic {
public:
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
};

//  Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
//  libraries to design a compiler (p. 17). Packt Publishing. Kindle Edition.

TEST_F(test_global_var, main)
{
    IRBuilder<> Builder(Context);
    auto ModuleOb = std::make_unique<Module>("my compiler", Context);

    GlobalVariable *gVar = createGlob(Builder, "x", ModuleOb.get());
    Function *fooFunc = createFunc(Builder, "foo", ModuleOb.get());
    BasicBlock *entry = createBB(fooFunc, "entry");
    Builder.SetInsertPoint(entry);
    Builder.CreateRet(Builder.getInt32(0));
    verifyFunction(*fooFunc);

    // Create workspace directory for output
    auto ws = create_workspace();
    auto output_file = ws / "output.ll";
    // Redirect output to file
    std::error_code EC;
    llvm::raw_fd_ostream output(output_file.string(), EC);
    ASSERT_FALSE(EC) << "Failed to open output file: " << EC.message();

    ModuleOb->print(output, nullptr, true, true);
    output.close();

    // Verify file was created and has content
    ASSERT_TRUE(fs::is_regular_file(output_file)) << "Output file was not created";
    EXPECT_LE(0, fs::file_size(output_file)) << "Output file is empty";
}

// Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM
// libraries to design a compiler (pp. 17-18). Packt Publishing. Kindle Edition.

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
