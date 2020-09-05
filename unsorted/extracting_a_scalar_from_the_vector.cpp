//
//
//
// Sarda, Suyog. LLVM Essentials: Become familiar with the LLVM infrastructure and start using LLVM libraries to design a compiler (pp. 48-49). Packt Publishing. Kindle Edition.
//

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <vector>
using namespace llvm;

static LLVMContext &Context = getGlobalContext();

