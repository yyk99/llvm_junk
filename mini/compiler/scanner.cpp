//
//
//

#include "scanner.h"

#include <iostream>
#include <FlexLexer.h>

yylval_t yylval;

int main(int arc, char **argv)
{
#if 0
    yyFlexLexer lexer;

    std::cout << lexer.yylex() << std::endl; 
    std::cout << lexer.yylex() << std::endl; 
    std::cout << lexer.yylex() << std::endl; 
    std::cout << lexer.yylex() << std::endl; 
#else
    extern int yylex();

    while(int next = yylex()) switch (next) {
    case NUMBER:
        std::cout << next <<  " -> " << yylval.num << std::endl;
        break;
    case IDENT:
        std::cout << next <<  " -> " << yylval.id << std::endl;
        break;
    default:
        std::cout << next <<  std::endl;
    } 
#endif
	return 0;
}
