//
//
//

#include <gtest/gtest.h>

#include "parser.h"
#include "parser_bits.h"
#include "TreeNode.h"
#include "llvm_helper.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"

#include <algorithm>
#include <cstdlib>
#include <stack>
#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

using namespace llvm;

LLVMContext C;
IRBuilder<> Builder(C);

class CompilerTestBase : public ::testing::Test
{
public:
    Module *TheModule;
    Function *F;

    CompilerTestBase()
    {
        static bool once;
        if(!once) {
            InitializeNativeTarget();
            InitializeNativeTargetAsmPrinter();

            once=true;
        }
    }

    std::string current_test_name () const
    {
        const ::testing::TestInfo * const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        return test_info->name();
    }

    std::string current_case_name () const
    {
        const ::testing::TestInfo * const test_info = ::testing::UnitTest::GetInstance()->current_test_info();
        return test_info->test_case_name();
    }
    
    virtual void SetUp() override {
        TheModule = new Module(current_case_name(), C);

        std::vector<Type *> formal_args(0);
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(C), formal_args, false);
        F = Function::Create(FT, Function::ExternalLinkage, current_test_name(), TheModule);
                                                                             
        BasicBlock *BB = llvm::BasicBlock::Create(C, "entry", F);
        Builder.SetInsertPoint(BB);
    }

    virtual void TearDown() override
    {
        Builder.CreateRet(Builder.getInt32(0));
        verifyFunction(*F);
        TheModule->print(errs(), nullptr);
        
        F->eraseFromParent();
        
        delete TheModule;
        TheModule = 0;
    }
};

class T2 : public CompilerTestBase {
};

TEST_F(T2, current_test_name)
{
    std::cout << "current_test_name: " << current_test_name() << std::endl;
}

TEST_F(T2, current_case_name)
{
    std::cout << "current_case_name: " << current_case_name() << std::endl;
}

TEST_F(T2, CreateArrayType)
{
    auto val = CreateArrayType(Type::getInt32Ty(C), 1);
    ASSERT_TRUE(val != 0);

    show_type_details(val);
}

TEST_F(T2, isArrayType)
{
    Type *array = CreateArrayType(Type::getInt32Ty(C), 1);    
    ASSERT_NE(nullptr, array);

    Value *val = Builder.CreateAlloca(array, 0, "array");
    ASSERT_TRUE(isArrayType(val));
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
