//
//
//
//

#include <iostream>

#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

void Module::dump() const
{
  print(llvm::errs(), nullptr);
}
