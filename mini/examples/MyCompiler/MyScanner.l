%{
#include "MyParser.hh"
#include <cstdlib>

// Define yylex to return the correct type
#define YY_DECL yy::MyParser::symbol_type yylex()
%}

%option noyywrap

digit	[0-9]
integer	[1-9]{digit}*|0
ws	[ \t\n]+

%%

{ws}        { /* no action */ }
{integer}   { return yy::MyParser::make_INTEGER(atoi(yytext)); }
"AND"       { return yy::MyParser::make_AND(); }
"OR"        { return yy::MyParser::make_OR(); }
"NOT"       { return yy::MyParser::make_NOT(); }
"TRUE"      { return yy::MyParser::make_BOOLEAN(true); }
"FALSE"     { return yy::MyParser::make_BOOLEAN(false); }
"-"         { return yy::MyParser::make_MINUS(); }
"+"         { return yy::MyParser::make_PLUS(); }
"("         { return yy::MyParser::make_LPARA(); }
")"         { return yy::MyParser::make_RPARA(); }
.           { std::cerr << "Unexpected character: " << yytext << std::endl; return yy::MyParser::make_YYerror(); }

%%
