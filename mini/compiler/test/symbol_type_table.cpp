//
//
//

#include <gtest/gtest.h>

#include "symbol_type_table.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

LLVMContext C;
IRBuilder<> Builder(C);

TEST(symbol_type_table, t1)
{
    symbol_type_table theTable;

    {
        auto actual = theTable.find("foo");
        ASSERT_TRUE(actual == 0);
    }

    {
        auto entry = new symbol_type("foo", 0, Type::getInt32Ty(C));
        bool r = theTable.insert(entry);
        ASSERT_TRUE(r);

        r = theTable.insert(entry);
        ASSERT_FALSE(r);
    }

    {
        auto actual = theTable.find("foo");
        ASSERT_TRUE(actual != 0);
        ASSERT_EQ("foo", actual->ident);
    }
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
