//
//
//

#ifndef __SCANNER_H
#define __SCANNER_H

#include <string>


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
	NUMBER,
	UNKNOWN,
};

//
// TODO: use std::variant to simulate union
//
struct yylval_t {
    std::string id;
    int num;
    double fnum;
};

extern yylval_t yylval;

#endif
