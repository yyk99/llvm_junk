//
//
//

#include <gtest/gtest.h>

#include "parser.h"

TEST(t3, token_to_string)
{
    std::cout << token_to_string(T_IS) << std::endl;
    ASSERT_EQ("PROGRAMSYM",  token_to_string(PROGRAMSYM));
    ASSERT_EQ("T_IS", token_to_string(T_IS));
    ASSERT_EQ("ARRAY", token_to_string(ARRAY));

    ASSERT_EQ("0", token_to_string(0));
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
