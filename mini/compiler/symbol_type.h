//
//
//

#ifndef __SYMBOL_TYPE_H
#define __SYMBOL_TYPE_H 1

#include <string>

namespace llvm {
    class Type;
};

class symbol_type {
public:
    std::string ident; /// 
    size_t size;
    size_t offset;
    llvm::Type *type;

    symbol_type(std::string id, size_t off, llvm::Type *t);
};

#endif
// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
