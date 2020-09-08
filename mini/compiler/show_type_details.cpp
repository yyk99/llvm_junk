//
//
//

#include "llvm/IR/DerivedTypes.h"
#if 0
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#endif

#include "llvm/Support/Error.h"
#include "llvm/Support/Debug.h"

#include "llvm_helper.h"


using namespace llvm;

void show_type_details(Type *t, std::string tab)
{
    if(t->isPointerTy()) {
        errs() << tab << "PointerTy\n";
        errs() << tab << "Points to: \n";
        show_type_details (t->getPointerElementType(), tab + "\t");
    } else if (t->isStructTy()) {
        StructType *st = cast<StructType>(t);
        errs() << tab << "StructTy\n";
        errs() << tab << "Total elements: " << st->getNumElements() << "\n";
        for( auto tt : st->elements()) {
            show_type_details(tt, tab + "\t");
        }
    } else if (t->isIntegerTy()) {
        errs() << tab << "IntegerTy\n";
    } else if (t->isDoubleTy()) {
        errs() << tab << "DoubleTy\n";
    } else {
        errs() << tab << "Other...\n";
    }
}

