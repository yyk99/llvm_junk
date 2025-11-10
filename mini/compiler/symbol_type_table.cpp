//
//
//

#include "symbol_type_table.h"

///
///
///
symbol_type const *
symbol_type_table::find(std::string const &id) const
{
    auto pos = _symbols.find(id);
    if(pos != _symbols.end())
        return pos->second;
    return 0;
}

///
///
///
bool
symbol_type_table::insert(symbol_type *entry)
{
    auto res = _symbols.insert(std::make_pair(entry->ident, entry));
    return res.second;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
