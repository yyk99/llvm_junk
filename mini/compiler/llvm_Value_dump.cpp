//
//
//

#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

namespace llvm {
#if 0
}
#endif

void Value::dump() const
{
    print(errs());
    errs() << "\n";
}

}; // namespace


// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
