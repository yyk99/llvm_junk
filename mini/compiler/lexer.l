%{
#include "parser.h"

int yylineno;

%}

digit         [0-9]
letter        [a-zA-Z]

%%
"+"                  { return PLUS;       }
"-"                  { return MINUS;      }
"*"                  { return TIMES;      }
"/"                  { return SLASH;      }
"("                  { return LPAREN;     }
")"                  { return RPAREN;     }
";"                  { return SEMICOLON;  }
","                  { return COMMA;      }
"."                  { return PERIOD;     }
":="                 { return BECOMES;    }
":"                  { return COLON; }
"="                  { return EQL;        }
"<>"                 { return NEQ;        }
"<"                  { return LSS;        }
">"                  { return GTR;        }
"<="                 { return LEQ;        }
">="                 { return GEQ;        }
"begin"              { return BEGINSYM;   }
"call"               { return CALLSYM;    }
"const"              { return CONSTSYM;   }
"do"                 { return DOSYM;      }
"end"                { return ENDSYM;     }
"if"                 { return IFSYM;      }
"fi"                 { return FISYM;      }
"odd"                { return ODDSYM;     }
"procedure"          { return PROCSYM;    }
"program"            { return PROGRAMSYM; }
"return"             { return RETURNSYM; }
"set"                { return SETSYM;     }
"then"               { return THENSYM;    }
"else"               { return ELSESYM; }
"while"              { return WHILE;   }
"by"        { return BY; }
"to"        { return TO; }
"repeat"    { return REPEAT; }
"repent"    { return REPENT; }
"input"     { return INPUT;}
"output"    { return OUTPUT;}
"select"    { return SELECT; }
"case"      { return CASE; }
"otherwise" { return OTHERWISE; }

{letter}({letter}|{digit})* {
                       /* yylval.id = yytext; */
                       return IDENT;      }
{digit}+             { yylval.num = atoi(yytext);
                       return NUMBER;     }
[\n]                 { ++yylineno; }
[ \t\r]            /* skip whitespace */
.                    { printf("Unknown character [%c]\n",yytext[0]);
                       return UNKNOWN;    }
%%

int yywrap(void){return 1;}