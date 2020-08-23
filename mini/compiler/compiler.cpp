//
//
//

#include <unistd.h>

#include "parser.h"
#include "parser_bits.h"

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

    init_compiler();
    
    int rc = yyparse();

    std::cerr << "Total: " << yylineno << " lines processed (" 
        << rc << ")" << std::endl;
	return rc;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:

