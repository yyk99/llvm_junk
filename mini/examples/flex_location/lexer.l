%{
#include "scanner.h"

// Define the YY_USER_ACTION macro to update the location before each rule's action
#define YY_USER_ACTION \
    yylloc->first_line = yylloc->last_line; \
    yylloc->first_column = yylloc->last_column; \
    for (int i = 0; i < yyleng; i++) { \
        if (yytext[i] == '\n') { \
            yylloc->last_line++; \
            yylloc->last_column = 0; \
        } else { \
            yylloc->last_column++; \
        } \
    }

%}

%option yyclass="MiniLexer"
%option c++
%option noyywrap
%option yylineno

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
.                    { std::cerr << "Error at " << yylloc->first_line + 1 << ":" << yylloc->first_column
                       << " : invalid character: " << yytext << std::endl;
                       return UNKNOWN; }
%%
