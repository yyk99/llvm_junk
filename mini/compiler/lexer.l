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
"procedure"          { return PROCSYM;    }
"program"            { return PROGRAMSYM; }
"return"             { return RETURNSYM; }
"set"                { return SETSYM;     }
"then"               { return THENSYM;    }
"else"               { return ELSESYM; }
"while"              { return WHILE;   }
"number"    { return NUMBERSYM; }
"xor"       { return XOR;}
"floor"     { return FLOOR; }
"fix"       { return FIX; }
"length"       { return LENGTH; }
"substr"       { return SUBSTR; }
"mod"       { return MOD; }
"by"        { return BY; }
"to"        { return TO; }
"repeat"    { return REPEAT; }
"repent"    { return REPENT; }
"input"     { return INPUT;}
"output"    { return OUTPUT;}
"select"    { return SELECT; }
"case"      { return CASE; }
"true"      { return TRUE; }
"false"     { return FALSE; }
"otherwise" { return OTHERWISE; }
"["         { return LBRACK; }
"]"         { return RBRACK; }

{letter}({letter}|{digit})* {
                               yylval.node = new TreeIdentNode(yytext); 
                               return IDENT;
                            }

{digit}+             { 
                       yylval.node = new TreeNumericalNode(atoi(yytext)); 
                       return NUMBER;
                     }

[\n]                 { ++yylineno; }
[ \t\r]            /* skip whitespace */
.                    { printf("Unknown character [%c]\n",yytext[0]);
                       return UNKNOWN;    }
"/*" {
    for (int c;;) {
        while ((c = yyinput()) != '*' && c != EOF)
            if(c == '\n') ++yylineno; /* eat up text of comment */
        if (c == '*') {
            while ((c = yyinput()) == '*')
                ;
            if (c == '/')
                break; /* found the end */
        }
        if (c == EOF) {
            yyerror ("EOF in comment");
            break;
        }
    }
}

%%

int yywrap(void){return 1;}
