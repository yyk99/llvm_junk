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
            std::cout << next << " -> " << lexer.num() << std::endl;
            break;
        case IDENT:
            std::cout << next << " -> " << lexer.id() << std::endl;
            break;
        case FNUMBER:
            std::cout << next << " -> " << lexer.fnum() << std::endl;
            break;
        default:
            std::cout << next << std::endl;
        }
    }
    return 0;
}
