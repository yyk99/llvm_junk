//
//
//

#if __has_include(<unistd.h>)
#   include <unistd.h>
#else
static int optind;

static int 
getopt(int argc, char **argv, const char *options)
{
    optind = 1;
    return -1;
}
#endif

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
        switch (opt) {
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

    argc -= optind;
    argv += optind;

    const char *input_file = "<stdin>";
    if (argc == 1)
        (void)freopen(input_file = argv[0], "r", stdin);

    init_compiler(input_file);

    int rc = yyparse();

    return rc;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
