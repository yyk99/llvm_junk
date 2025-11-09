%{
#include "MyParser.h"
#include <cstdlib>
%}

digit	[0-9]
integer	[1-9]{digit}*
ws	[ \t\n]+

%%

{ws}        { /* no action */ }
{integer}   { yylval.num = atoi(yytext); return INTEGER; }
"AND"       { return AND; }
"OR"        { return OR; }
"NOT"       { return NOT; }
"TRUE"      { yylval.statement = true; return BOOLEAN; }
"FALSE"     { yylval.statement = false; return BOOLEAN; }
"-"         { return MINUS; }
"+"         { return PLUS; }
"("         { return LPARA; }
")"         { return RPARA; }

%%

int yywrap()
{
    return 1;
}