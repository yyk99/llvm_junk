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
    while ((opt = getopt(argc, argv, "dv")) != -1) {
        switch(opt){
        case 'd':
#ifdef YYDEBUG
            yydebug = 1;
#endif
            break;
        case 'v':
            flag_verbose = true;
            break;
        }
    }

    init_compiler();
    
    int rc = yyparse();

	return rc;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:

