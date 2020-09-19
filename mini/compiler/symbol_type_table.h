//
//
//

#ifndef SYMBOL_TYPE_TABLE_H
#define SYMBOL_TYPE_TABLE_H

#include "symbol_type.h"

#include <string>
#include <unordered_map>

class symbol_type_table {
    std::unordered_map<std::string, symbol_type *> _symbols;
public:

    symbol_type const *find(std::string const &id) const;
    bool insert(symbol_type *entry);
};

#endif

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
