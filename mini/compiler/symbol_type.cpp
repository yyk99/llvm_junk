//
//
//

#include "symbol_type.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>

#include <cassert>

using namespace llvm;

symbol_type::symbol_type(std::string id, size_t off, llvm::Type *t)
    : ident(id), size(0), offset(off), type(t)
{
    auto sz = ConstantExpr::getSizeOf(type);
    assert(sz != 0);
}


// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
