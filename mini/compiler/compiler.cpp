//
//
//

#if __has_include(<unistd.h>)
#   include <unistd.h>
#else
static int optind = 1;
static int optopt = 0;

static int
getopt(int argc, char **argv, const char *options)
{
    // No more arguments or reached end of options
    if (optind >= argc || argv[optind] == nullptr || argv[optind][0] != '-' || argv[optind][1] == '\0') {
        return -1;
    }

    // End of options marker "--"
    if (argv[optind][0] == '-' && argv[optind][1] == '-' && argv[optind][2] == '\0') {
        optind++;
        return -1;
    }

    // Get the option character
    char const opt = argv[optind][1];
    optopt = opt;

    // Look for the option in the options string
    char const *p = options;
    while (*p && *p != opt) {
        p++;
    }

    // Move to next argument for next call
    optind++;

    // Return the option if found, otherwise '?'
    return (*p == opt) ? opt : '?';
}
#endif

#include "parser.h"
#include "parser_bits.h"

extern int yyparse();
extern int yylineno;

static void
usage()
{
    fprintf(stderr, "Usage: compiler [-v] [-d] [-h] filename.mini\n");
}

int
main(int argc, char **argv)
{
#ifdef YYDEBUG
    extern int yydebug;
#endif

    int opt;
    while ((opt = getopt(argc, argv, "dvh")) != -1) {
        switch (opt) {
        case 'd':
#ifdef YYDEBUG
            yydebug = 1;
#endif
            break;
        case 'v':
            flag_verbose = true;
            break;
        case 'h':
            usage();
            return 1;
        }
    }

    argc -= optind;
    argv += optind;

    const char* input_file = "<stdin>";
    if (argc == 1) {
        if (freopen(input_file = argv[0], "r", stdin) == NULL) {
            fprintf(stderr, "Input file open error\n");
            return 1;
        }
    }
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
