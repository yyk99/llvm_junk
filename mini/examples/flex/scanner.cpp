//
//
//

#include <iostream>
#include <FlexLexer.h>

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

    while(int next = yylex())
        std::cout << next <<  std::endl; 
#endif
	return 0;
}
