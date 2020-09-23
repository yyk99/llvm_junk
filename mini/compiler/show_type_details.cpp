// show_type_details.cpp - Utility function for displaying LLVM type information
// Used for debugging and analyzing LLVM IR types in the Mini compiler

#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/Debug.h"

#include "llvm_helper.h"

using namespace llvm;

// Recursively display detailed information about an LLVM type
// Parameters:
//   t   - The LLVM type to analyze
//   tab - Indentation prefix for nested output (defaults to empty string)

void show_type_details(Type *t, std::string tab)
{
    if (t->isPointerTy()) {
        errs() << tab << "PointerTy (opaque)\n";
        // With opaque pointers in LLVM 15+, we cannot determine the pointee type
    } else if (t->isStructTy()) {
        StructType *st = cast<StructType>(t);
        errs() << tab << "StructTy";
        if (st->hasName()) {
            errs() << " (name: " << st->getName() << ")";
        }
        errs() << "\n";
        errs() << tab << "Total elements: " << st->getNumElements() << "\n";
        for (auto tt : st->elements()) {
            show_type_details(tt, tab + "    ");
        }
    } else if (t->isArrayTy()) {
        ArrayType *ar = cast<ArrayType>(t);
        errs() << tab << "ArrayTy\n";
        errs() << tab << "Total elements: " << ar->getNumElements() << "\n";
        show_type_details(ar->getElementType(), tab + "    ");
    } else if (t->isIntegerTy()) {
        IntegerType *it = cast<IntegerType>(t);
        errs() << tab << "IntegerTy (i" << it->getBitWidth() << ")\n";
    } else if (t->isFloatingPointTy()) {
        if (t->isFloatTy()) {
            errs() << tab << "FloatTy (32-bit)\n";
        } else if (t->isDoubleTy()) {
            errs() << tab << "DoubleTy (64-bit)\n";
        } else if (t->isHalfTy()) {
            errs() << tab << "HalfTy (16-bit)\n";
        } else if (t->isBFloatTy()) {
            errs() << tab << "BFloatTy (16-bit)\n";
        } else if (t->isFP128Ty()) {
            errs() << tab << "FP128Ty (128-bit)\n";
        } else {
            errs() << tab << "FloatingPointTy (other)\n";
        }
    } else if (t->isVectorTy()) {
        VectorType *vt = cast<VectorType>(t);
        errs() << tab << "VectorTy";
        if (auto *fvt = dyn_cast<FixedVectorType>(vt)) {
            errs() << " (fixed, " << fvt->getNumElements() << " elements)";
        } else {
            errs() << " (scalable)";
        }
        errs() << "\n";
        show_type_details(vt->getElementType(), tab + "    ");
    } else if (t->isFunctionTy()) {
        FunctionType *ft = cast<FunctionType>(t);
        errs() << tab << "FunctionTy\n";
        errs() << tab << "Return type:\n";
        show_type_details(ft->getReturnType(), tab + "    ");
        errs() << tab << "Parameters (" << ft->getNumParams() << "):\n";
        for (auto pt : ft->params()) {
            show_type_details(pt, tab + "    ");
        }
        if (ft->isVarArg()) {
            errs() << tab << "    ...\n";
        }
    } else if (t->isVoidTy()) {
        errs() << tab << "VoidTy\n";
    } else if (t->isLabelTy()) {
        errs() << tab << "LabelTy\n";
    } else if (t->isMetadataTy()) {
        errs() << tab << "MetadataTy\n";
    } else if (t->isTokenTy()) {
        errs() << tab << "TokenTy\n";
    } else {
        errs() << tab << "UnknownTy (ID: " << t->getTypeID() << ")\n";
    }
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
