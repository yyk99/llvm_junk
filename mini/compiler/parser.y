%{
%}
%name Mini
%header{
#include <iostream>
#include <string>

#include "TreeNode.h"

#define YYDEBUG 1

void yyerror(const char *s); 
int yylex();

%}

%union {
  TreeNode *node;
  int num;
  }


/*
%token <num> PLUS INTEGER MINUS  AND OR NOT LPARA RPARA
%token <statement> BOOLEAN
%type <num> exp result
%type <statement> bexp
*/
%token PROGRAMSYM IDENT COLON ENDSYM BEGINSYM ENDSYM SEMICOLON RETURNSYM
%token WHILE ELSESYM REPENT REPEAT BY TO FOR SELECT OF OTHERWISE CASE
%token SETSYM FISYM EXITSYM INPUT OUTPUT
%token PLUS MINUS TIMES SLASH LPAREN RPAREN SEMICOLON COMMA PERIOD BECOMES EQL NEQ LSS GTR
%token LEQ GEQ BEGINSYM CALLSYM CONSTSYM DOSYM
%token ENDSYM IFSYM ODDSYM PROCSYM THENSYM VARSYM
%token WHILESYM IDENT NUMBER UNKNOWN %token PLUS INTEGER MINUS  AND OR NOT LPARA RPARA

%start compiler_unit

%left OR
%left AND
%left PLUS MINUS
%left NOT
%left LPARA RPARA

%%
compiler_unit   : program_segment
                | compiler_unit program_segment

program_segment : main_program
/*                | external_procedure */

main_program    : program_header program_body program_end

program_header  : PROGRAMSYM IDENT COLON { ; }

program_body    : segment_body

program_end     : ENDSYM PROGRAMSYM

/* segments */
segment_body    : executable_statements

executable_statements   : executable_statement
                        | executable_statement executable_statements

executable_statement    : assign_statement
                        | call_statement
                        | return_statement
                        | exit_statement
                        | conditional_statement
                        | compound_statement
                        | loop_statement
                        | select_statement
                        | repeat_statement
                        | repent_statement
                        | input_statement
                        | output_statement
                        | empty_statement

empty_statement         : SEMICOLON

repeat_statement        : REPEAT IDENT SEMICOLON

repent_statement        : REPENT IDENT SEMICOLON

compound_statement      : simple_compound_statement
                        | label simple_compound_statement

simple_compound_statement : compound_header compound_body compound_footer

compound_header         : BEGINSYM

compound_body           : segment_body

compound_footer         : ENDSYM SEMICOLON
                        | ENDSYM IDENT SEMICOLON

call_statement          : CALLSYM IDENT SEMICOLON

exit_statement          : EXITSYM SEMICOLON

conditional_statement   : simple_cond_statement
                        | label simple_cond_statement

simple_cond_statement   : cond_specification true_branch FISYM SEMICOLON
                        | cond_specification true_branch false_branch FISYM SEMICOLON

cond_specification      : IFSYM expr

true_branch             : THENSYM cond_statement_body

false_branch            : ELSESYM cond_statement_body

cond_statement_body     : segment_body


/* select */
select_statement        : simple_select_statement
                        | label simple_select_statement
simple_select_statement : select_header select_body select_footer
select_header           : SELECT expr OF
select_body             : case_list
                        | case_list other_cases
select_footer           : ENDSYM SELECT SEMICOLON
                        | ENDSYM SELECT IDENT SEMICOLON
case_list               : case
                        | case case_list
case                    : case_head case_body
                         
case_head               : CASE selector COLON
selector                : selector_head
                        | selector_head selector

selector_head           : expr

other_cases             : other_header case_body

other_header            : OTHERWISE COLON

case_body               : segment_body

return_statement        : RETURNSYM SEMICOLON
                        | RETURNSYM expr SEMICOLON

assign_statement        : SETSYM target_list expr SEMICOLON

target_list             : target
                        | target_list target 

target                  : variable BECOMES


/* loop */
loop_statement          : simple_loop_statement
                        | label simple_loop_statement

simple_loop_statement   : loop_head loop_body loop_footer

loop_head               : for loop_target control DOSYM

loop_body               : segment_body

loop_footer             : ENDSYM FOR SEMICOLON
                        | ENDSYM FOR IDENT SEMICOLON

for                     : FOR

loop_target             : variable BECOMES

control                 : step_control
                        | step_control cond_control

step_control            : start_value step
                        | start_value limit
                        | start_value step limit

start_value             : expr
step                    : BY expr
limit                   : TO expr
cond_control            : WHILE expr

input_statement         : INPUT input_list
input_list              : variable
                        | variable COMMA input_list


output_statement        : OUTPUT output_list
output_list             : expr
                        | expr COMMA output_list
label                   : IDENT COLON

/* expr */
expr                    : NUMBER

variable                : IDENT

/*
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
*/
%%
/* -------------- body section -------------- */
// feel free to add your own C/C++ code here

// extern 
extern int yylineno;
void yyerror(const char *s) {
    fprintf(stderr, " line %d: %s\n", yylineno, s);
}
