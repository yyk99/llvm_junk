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

extern LLVMContext TheContext;
extern IRBuilder<> Builder;

class CompilerTestBase : public ::testing::Test
{
public:
    LLVMContext &C;
    
    Module *TheModule;
    Function *F;

    bool verbose;

    CompilerTestBase() : C(TheContext), verbose{false}
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

        set_current_function(F);
        
        BasicBlock *BB = llvm::BasicBlock::Create(C, "entry", F);
        Builder.SetInsertPoint(BB);
    }

    virtual void TearDown() override
    {
        Builder.CreateRet(Builder.getInt32(0));
        verifyFunction(*F);

        if(verbose)
            TheModule->print(errs(), nullptr);

        functions_pop();
        
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
    auto type = CreateArrayType(Type::getInt32Ty(C), 1);
    ASSERT_TRUE(type != 0);

    show_type_details(type);
    type->dump();
}

TEST_F(T2, get_current_function)
{
    Function *actual = get_current_function();
    ASSERT_TRUE(actual != 0);
}


TEST_F(T2, CreateArrayType2)
{
    std::vector<Type *> types;

    int ndims = 2;

    Type *vecTy = ArrayType::get(Type::getInt32Ty(C), 3 * ndims);
    types.push_back(vecTy);
    Type *ptr = PointerType::getUnqual(Type::getInt32Ty(C));
    types.push_back(ptr);

    Type *type = StructType::get(C, TypeArray(types));
    
    Value *array = Builder.CreateAlloca(type, 0, "array");
    ASSERT_TRUE(isArrayType(array));

    // emit initialization for the array variable
    auto Low = Builder.getInt32(1);
    auto Up = Builder.getInt32(10);
    auto Zero = Builder.getInt32(0);
    auto One = Builder.getInt32(1);

    Value *header = Builder.CreateStructGEP(type, array, 0);
    ASSERT_TRUE(header != 0);

    show_type_details(header->getType());

    auto *arr_type = cast<ArrayType>(cast<StructType>(type)->getElementType(0));
    Value *pos0 = Builder.CreateGEP(arr_type, header, {Zero, Zero});
    Builder.CreateStore(Low, pos0);

    Value *pos1 = Builder.CreateGEP(arr_type, header, {Zero, One});
    Builder.CreateStore(Up, pos1);
}

TEST_F(T2, CreateArrayType3)
{
    std::vector<Type *> types;

    int ndims = 2;

    Type *vecTy = ArrayType::get(Type::getInt32Ty(C), 3 * ndims);
    types.push_back(vecTy);
    Type *ptr = PointerType::getUnqual(Type::getInt32Ty(C));
    types.push_back(ptr);

    Type *type = StructType::get(C, TypeArray(types));

    Value *header = Builder.CreateAlloca(type, 0, "array");
    ASSERT_TRUE(isArrayType(header));

    // emit initialization for the array variable
    auto Low = Builder.getInt32(1);
    auto Up = Builder.getInt32(10);
    auto Zero = Builder.getInt32(0);
    auto One = Builder.getInt32(1);

    auto *struct_type = cast<StructType>(type);
    Value *pos0 = Builder.CreateGEP(struct_type, header, {Zero, Zero, Zero});
    Builder.CreateStore(Low, pos0);

    Value *pos1 = Builder.CreateGEP(struct_type, header, {Zero, Zero, One});
    Builder.CreateStore(Up, pos1);
}

TEST_F(T2, isArrayType)
{
    Type *array = CreateArrayType(Type::getInt32Ty(C), 1);    
    ASSERT_NE(nullptr, array);

    Value *val = Builder.CreateAlloca(array, 0, "array");
    ASSERT_TRUE(isArrayType(val));
}

TEST_F(T2, node_to_type_structure)
{
    ASSERT_TRUE(Builder.GetInsertBlock() != 0);
    // 0. create
    Value *dummy = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "dummy");
    
    // 1. construct TreeNode
    // [STRUCTURE,COMMA(FIELD(first T_REAL(<null>)) FIELD(second T_REAL(<null>))),<null>]

    auto *_1 = make_binary(new TreeIdentNode("first"), base_type(T_REAL), FIELD);
    auto *_3 = make_binary(new TreeIdentNode("second"), base_type(T_REAL), FIELD); 
    auto *_2 = make_binary(_1, _3, COMMA);
    auto *s_node = make_binary(_2, 0, STRUCTURE);

    std::cout << "s_node: " << s_node->show() << "\n";

    // 2. call it
    // type_value_t NodeToType(TreeNode *node, const char *sym)
    // typedef std::pair<llvm::Type *, llvm::Value *> type_value_t;
    ASSERT_TRUE(Builder.GetInsertBlock() != 0);
    type_value_t actual = node_to_type(s_node, "foo");

    // 3. Verify
    ASSERT_TRUE(actual.first != 0);
    Type *type = actual.first;
    ASSERT_TRUE(actual.second != 0);
    Value *val = actual.second;

    show_type_details(type);
    type->dump();
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
