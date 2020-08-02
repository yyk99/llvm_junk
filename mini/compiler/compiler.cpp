//
//
//

#include "parser.h"

extern int yyparse();
extern int yylineno;

int main(int arc, char **argv)
{

    extern int yydebug;

//     yydebug = 1;

    int rc = yyparse();

    std::cout << "Total: " << yylineno << " lines processed (" 
        << rc << ")" << std::endl;
	return rc;
}
