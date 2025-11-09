%{
#define YY_USE_CLASS 1
#include <iostream>
#include <cstdio>
using namespace std;

// Forward declarations for standalone yyparse() compatibility
int yylex();
void yyerror(char *);

// Provide default implementations for virtual methods
#define YY_MyParser_LEX_BODY { return yylex(); }
#define YY_MyParser_ERROR_BODY { yyerror(msg); }
%}

%name MyParser

%union {
  int num;
  bool statement;
}

%{
// Global yylval variable for scanner
yy_MyParser_stype yylval;
%}



%token PLUS MINUS AND OR NOT LPARA RPARA
%token <num> INTEGER
%token <statement> BOOLEAN
%type <num> exp
%type <statement> bexp
%start result

%left OR
%left AND
%left PLUS MINUS
%left NOT
%left LPARA RPARA

%%

result          : exp {cout << "Result = " << $1 << endl;}
                | bexp {cout << "Result = " << $1 << endl;}

exp		: exp PLUS exp {$$ = $1 + $3;}
       		| INTEGER {$$ = $1;}
                | MINUS exp { $$ = -$2;}
                | exp MINUS exp {$$ = $1 - $3;}

bexp            : BOOLEAN {$$ = $1;}
		| bexp AND bexp { $$ = $1 && $3;}
		| bexp OR bexp { $$ = $1 || $3;}
		| NOT bexp {$$ = !$2;}
		| LPARA bexp RPARA {$$ = $2}
%%
/* -------------- body section -------------- */
// feel free to add your own C/C++ code here
