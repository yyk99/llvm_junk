//
//
//

#include "parser.h"

extern int yyparse();
extern int yylineno;

int main(int arc, char **argv)
{
#if 1
    int rc = yyparse();
#else
    while(int c = yylex()) {
        std::cout << c << std::endl;
    }
#endif
    std::cout << "Total: " << yylineno << " lines processed" << std::endl;
	return 0;
}
