#define YY_USE_CLASS 1
#include "MyParser.h"
#include <cstdio>

extern int yylex();
extern FILE* yyin;

class MyCompiler : public MyParser
{
public:
    virtual int yylex();
    virtual void yyerror(char *m);
};

int MyCompiler::yylex()
{
    return ::yylex();
}

void MyCompiler::yyerror(char *m)
{
    fprintf(stderr, "Parse error: %s\n", m);
}

// Standalone functions for C-style yyparse() compatibility
void yyerror(char *m)
{
    fprintf(stderr, "Parse error: %s\n", m);
}

int main(int argc, char **argv)
{
    MyCompiler compiler;
    int result = compiler.yyparse();
    printf("Parsing result: %s\n", result ? "Error" : "OK");
    return result;
}

