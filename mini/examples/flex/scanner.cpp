//
//
//

#include "scanner.h"

#include <iostream>
#include <FlexLexer.h>

yylval_t yylval;

int main(int arc, char **argv)
{
    yyFlexLexer lexer;
    while (int next = lexer.yylex()) {
        switch (next) {
        case NUMBER:
            std::cout << next << " -> " << yylval.num << std::endl;
            break;
        case IDENT:
            std::cout << next << " -> " << yylval.id << std::endl;
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
