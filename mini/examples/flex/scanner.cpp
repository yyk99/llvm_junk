//
//
//

#include <FlexLexer.h>
#include "scanner.h"

#include <iostream>

int main(int arc, char **argv)
{
    MiniLexer lexer;
    while (int next = lexer.yylex()) {
        switch (next) {
        case NUMBER:
            std::cout << next << " -> " << lexer.yylval.num << std::endl;
            break;
        case IDENT:
            std::cout << next << " -> " << lexer.yylval.id << std::endl;
            break;
        default:
            std::cout << next << std::endl;
        }
    }
    return 0;
}

int yyFlexLexer::yywrap(void)
{
    return 1;  // Return 1 to indicate end of input
}
