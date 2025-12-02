//
//
//

#ifndef __SCANNER_H
#define __SCANNER_H

#include <string>
#include <variant>

enum symbol_t {
    PLUS=1,
    MINUS,
    TIMES,
    SLASH,
    LPAREN,
    RPAREN,
    SEMICOLON,
    COMMA,
    PERIOD,
    BECOMES,
    EQL,
    NEQ,
    LSS,
    GTR,
    LEQ,
    GEQ,
    BEGINSYM,
    CALLSYM,
    CONSTSYM,
    DOSYM,
    ENDSYM,
    IFSYM,
    ODDSYM,
    PROCSYM,
    THENSYM,
    VARSYM,
    WHILESYM,
    IDENT,
    NUMBER,  // integer
    FNUMBER, // double
    UNKNOWN,
};

struct location_t {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};

class MiniLexer : public yyFlexLexer {
public:
    typedef std::variant<std::string, int, double> yylval_t;

    yylval_t yylval;

    location_t *yylloc;

public:
    MiniLexer()
        : yylloc (new location_t{})
    {
    }

    ~MiniLexer()
    {
        delete yylloc;
    }

    int yylex() override;

    int num() const { return std::get<int>(yylval); }
    std::string id() const { return std::get<std::string>(yylval); }
    double fnum() const { return std::get<double>(yylval); }
};

#endif
