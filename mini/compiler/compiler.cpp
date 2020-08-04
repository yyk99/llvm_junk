//
//
//

#include <unistd.h>

#include "parser.h"

extern int yyparse();
extern int yylineno;

int main(int argc, char **argv)
{
#ifdef YYDEBUG
    extern int yydebug;
#endif

    int opt;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch(opt){
        case 'd':
#ifdef YYDEBUG
            yydebug = 1;
#endif
            break;
        }
    }

    int rc = yyparse();

    std::cout << "Total: " << yylineno << " lines processed (" 
        << rc << ")" << std::endl;
	return rc;
}
