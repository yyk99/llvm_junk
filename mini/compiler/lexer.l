%option yylineno

%{
#include "parser.h"
#include <cstring>

#if ! __has_include(<unistd.h>)
#    define YY_NO_UNISTD_H
#    include <io.h> /* for isatty */
#endif
%}

digit         [0-9]
letter        [a-zA-Z_]

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
"array" { return ARRAY; }
"structure" { return STRUCTURE; }
"field" { return FIELD; }
"of" { return OF; }
"begin"              { return BEGINSYM;   }
"call"               { return CALLSYM;    }
"do"                 { return DOSYM;      }
"for" { return FOR; }
"end"                { return ENDSYM;     }
"if"                 { return IFSYM;      }
"fi"                 { return FISYM;      }
"external" {return EXTERNAL; }
"name" { return NAME; }
"procedure"          { return T_PROCEDURE; }
"function" { return T_FUNCTION; }
"program"            { return PROGRAMSYM; }
"return"             { return RETURNSYM; }
"set"                { return SETSYM;     }
"then"               { return THENSYM;    }
"else"               { return ELSESYM; }
"while"              { return WHILE;   }
"number"    { return NUMBERSYM; }
"xor"       { return XOR;}
"and"       { return AND; }
"or"        { return OR; }
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
"declare"   { return T_DECLARE; }
"is"        { return T_IS; }
"type"      { return T_TYPE; }
"real"      { return T_REAL; }
"float" { return FLOAT; }
"boolean"   { return T_BOOLEAN;}
"string"    { return T_STRING;}
"integer"   { return T_INTEGER;}


{letter}({letter}|{digit})* {
                               yylval.node = new TreeIdentNode(yytext); 
                               return IDENT;
                            }

{digit}+             { 
                       yylval.node = new TreeNumericalNode(atoi(yytext)); 
                       return NUMBER;
                     }

{digit}+[.]{digit}* {
                       yylval.node = new TreeDNumericalNode(atof(yytext));
                       return NUMBER;
                    }


\"[^\"]*\"    { yylval.node = new TreeTextNode(yytext+1, strlen(yytext) - 2); return TEXT; }

[\n]                 { ++yylineno; }
[ \t\r]            /* skip whitespace */
.                    { printf("Unknown character [%c]\n",yytext[0]);
                       return UNKNOWN; }
"/*" {
    for (int c;;) {
        while ((c = yyinput()) != '*' && c != EOF)
            if(c == '\n') ++yylineno; /* eat up text of comment */
        if (c == '*') {
            while ((c = yyinput()) == '*')
                ;
            if (c == '/')
                break; /* found the end */
            if (c == '\n')
                ++yylineno;
        }
        if (c == EOF) {
            yyerror ("EOF in comment");
            break;
        }
    }
}

%%

int yywrap(void){return 1;}


// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
