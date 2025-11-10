%skeleton "lalr1.cc"
%require "3.0"
%defines
%define api.namespace {yy}
%define api.parser.class {MyParser}
%define api.value.type variant
%define api.token.constructor
%define parse.assert

%code requires {
#include <iostream>
#include <string>
using namespace std;
}

%code {
#include <cstdio>

// Declare yylex - will be provided by flex-generated scanner
extern yy::MyParser::symbol_type yylex();
}

%token PLUS MINUS AND OR NOT LPARA RPARA
%token <int> INTEGER
%token <bool> BOOLEAN
%type <int> exp
%type <bool> bexp
%start result

%left OR
%left AND
%left PLUS MINUS
%left NOT
%left LPARA RPARA

%%

result          : exp {cout << "Result = " << $1 << endl;}
                | bexp {cout << "Result = " << $1 << endl;}
                ;

exp		: exp PLUS exp {$$ = $1 + $3;}
       		| INTEGER {$$ = $1;}
                | MINUS exp { $$ = -$2;}
                | exp MINUS exp {$$ = $1 - $3;}
                ;

bexp            : BOOLEAN {$$ = $1;}
		| bexp AND bexp { $$ = $1 && $3;}
		| bexp OR bexp { $$ = $1 || $3;}
		| NOT bexp {$$ = !$2;}
		| LPARA bexp RPARA {$$ = $2;}
                ;

%%

namespace yy {
    void MyParser::error(const std::string& msg) {
        std::cerr << "Parse error: " << msg << std::endl;
    }
}
