%{
#include "scanner.h"
%}

%option yyclass="MiniLexer"

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
"odd"                { return ODDSYM;     }
"procedure"          { return PROCSYM;    }
"then"               { return THENSYM;    }
"var"                { return VARSYM;     }
"while"              { return WHILESYM;   }
{letter}({letter}|{digit})* {
                       yylval = yytext;
                       return IDENT;      }
{digit}+             { yylval = atoi(yytext);
                       return NUMBER;     }
{digit}+[.]{digit}+  { yylval = atof(yytext);
                       return FNUMBER;     }                    
[ \t\n\r]            /* skip whitespace */
.                    { printf("Unknown character [%c]\n",yytext[0]);
                       return UNKNOWN;    }
%%

int yywrap(void){return 1;}
