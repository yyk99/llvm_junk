//
//
//

#include <gtest/gtest.h>

#include "symbol_type_table.h"
#include "parser_bits.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

extern LLVMContext TheContext;
extern IRBuilder<> Builder;

TEST(symbol_type_table, t1)
{
    symbol_type_table theTable;

    {
        auto actual = theTable.find("foo");
        ASSERT_TRUE(actual == 0);
    }

    {
        auto entry = new symbol_type("foo", 0, Type::getInt32Ty(TheContext));
        bool r = theTable.insert(entry);
        ASSERT_TRUE(r);

        r = theTable.insert(entry);
        ASSERT_FALSE(r);
    }

    {
        auto actual = theTable.find("foo");
        ASSERT_TRUE(actual);
        ASSERT_EQ("foo", actual->ident);
    }
}

TEST(CreateStructType, t1)
{
    Type *a = Type::getInt32Ty(TheContext);
    Type *b = Type::getInt32Ty(TheContext);
    Type *actual = CreateStructType({a, b}, "foo_t");
    ASSERT_TRUE(actual);
    actual->dump();


    StructType *stype = cast<StructType>(actual);
    if (!stype->isLiteral())
        stype->setName("s1");
    stype->dump();
}

TEST(CreateStructType, t2)
{
    Type *a = Type::getInt32Ty(TheContext);
    Type *b = Type::getInt32Ty(TheContext);
    Type *s1 = CreateStructType({a, b}, "foo_t");
    s1->dump();

    Type *c = Type::getFloatTy(TheContext);
    Type *actual = CreateStructType({c, s1}, "bar_t");
    ASSERT_TRUE(actual);
    actual->dump();
}

TEST(CreateStructType, t3)
{
    Type *a = Type::getInt32Ty(TheContext);
    Type *b = Type::getInt32Ty(TheContext);
    Type *actual = CreateStructType({a, b}, "foo_t");
    ASSERT_TRUE(actual);
    actual->dump();

    StructType *stype = cast<StructType>(actual);
    ASSERT_TRUE(stype);
    EXPECT_TRUE(stype->hasName());
    errs() << "stype->getName(): " << stype->getName() << "\n";

    stype->setName("s1");
    EXPECT_TRUE(stype->hasName());
    errs() << "stype->getName(): " << stype->getName() << "\n";
    stype->dump();
}

//
// get const value from Value
// 
TEST(Value, value)
{
    Value *value = Builder.getInt32(42);

    errs() << "value = "; value->dump();

    ASSERT_TRUE(isa<ConstantInt>(value));

    ConstantInt *cint = cast<ConstantInt>(value);
    ASSERT_TRUE(cint);

    size_t actual = (size_t)cint->getLimitedValue();

    ASSERT_EQ(42, actual);

    // ...
}

TEST(Value, getSizeOf)
{
    Value *value = Builder.getInt32(42);
    // NOTE: ConstantExpr::getSizeOf returns Constant *
    Value *szValue = ConstantExpr::getSizeOf(value->getType());

    errs() << "value = "; value->dump();
    errs() << "szValue = "; szValue->dump();
    
    ASSERT_TRUE(isa<Constant>(szValue));

    Constant *cint = cast<Constant>(szValue);
    ASSERT_TRUE(cint != 0);

    // size_t actual = (size_t)cint->getLimitedValue();
    // ASSERT_EQ(4, actual);

    // auto api = cint->getUniqueInteger();
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
