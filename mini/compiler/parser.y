%{
%}
%name Mini
%header{
#include <iostream>
#include <string>

#include "TreeNode.h"

#include "parser_bits.h"
  
void yyerror(const char *s); 
int yylex();

TreeNode *make_ident(TreeNode *p1);

%}


%union {
  int num;
  char *text;
  TreeNode *node;
}

%token <node> PROGRAMSYM COLON RETURNSYM
%token <node> WHILE ELSESYM REPENT REPEAT BY TO FOR SELECT OF OTHERWISE CASE
%token <node> SETSYM FISYM EXITSYM INPUT OUTPUT XOR
%token <node> TIMES SLASH LPAREN RPAREN SEMICOLON COMMA PERIOD BECOMES EQL NEQ LSS GTR
%token <node> LEQ GEQ BEGINSYM CALLSYM CONSTSYM DOSYM
%token <node> ENDSYM IFSYM PROCSYM THENSYM VARSYM
%token <node> IDENT NUMBER UNKNOWN LBRACK RBRACK
%token <node> PLUS INTEGER MINUS AND OR NOT
%token <node> CONCAT /* || */
%token <node> FLOOR LENGTH SUBSTR CHARACTER NUMBERSYM FIX MOD TRUE FALSE TEXT FLOAT


%type <node> compiler_unit   
%type <node> program_segment 
%type <node> main_program    
%type <node> program_header  
%type <node> program_body    
%type <node> program_end     
%type <node> segment_body    
%type <node> executable_statements   
%type <node> executable_statement    
%type <node> empty_statement         
%type <node> repeat_statement        
%type <node> repent_statement        
%type <node> compound_statement      
%type <node> simple_compound_statement 
%type <node> compound_header         
%type <node> compound_body           
%type <node> compound_footer         
%type <node> call_statement          
%type <node> exit_statement          
%type <node> conditional_statement   
%type <node> simple_cond_statement   
%type <node> cond_specification      
%type <node> true_branch             
%type <node> false_branch            
%type <node> cond_statement_body     
%type <node> select_statement        
%type <node> simple_select_statement 
%type <node> select_header           
%type <node> select_body             
%type <node> select_footer           
%type <node> case_list               
%type <node> case                    
%type <node> case_head               
%type <node> selector                
%type <node> selector_head           
%type <node> other_cases             
%type <node> other_header            
%type <node> case_body               
%type <node> return_statement        
%type <node> assign_statement        
%type <node> target_list             
%type <node> target                  
%type <node> loop_statement          
%type <node> simple_loop_statement   
%type <node> loop_head               
%type <node> loop_body               
%type <node> loop_footer             
%type <node> for                     
%type <node> loop_target             
%type <node> control                 
%type <node> step_control            
%type <node> start_value             
%type <node> step                    
%type <node> limit                   
%type <node> cond_control            
%type <node> input_statement         
%type <node> input_list              
%type <node> output_statement        
%type <node> output_list             
%type <node> label                   
%type <node> expr                    
%type <node> expr1                   
%type <node> expr2                   
%type <node> expr3                   
%type <node> expr4                   
%type <node> expr5   
%type <node> expr_unary  
%type <node> expr6   
%type <node> expr7   
%type <node> expr8   
%type <node> relation_op 
%type <node> mult_op     
%type <node> constant    
%type <node> function_call 
%type <node> function_ident   
%type <node> actual_params   
%type <node> actual_params_header 
%type <node> variable    

%start compiler_unit

%%
compiler_unit   : program_segment
                | compiler_unit program_segment

program_segment : main_program
/*                | external_procedure */

main_program    : program_header program_body program_end

program_header  : PROGRAMSYM IDENT COLON { program_header($2); }

program_body    : segment_body

program_end     : ENDSYM PROGRAMSYM IDENT SEMICOLON { program_end($3); }

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
expr                    : expr1
                        | expr1 OR expr { std::cout << "OR" << std::endl; }
                        | expr1 XOR expr { std::cout << "XOR" << std::endl; }

expr1                   : expr2
                        | expr2 AND expr1 { std::cout << "AND" << std::endl; }

expr2                   : expr3
                        | NOT expr3

expr3                   : expr4
                        | expr4 relation_op expr3
expr4                   : expr5
                        | expr5 CONCAT expr4
expr5   : expr_unary
        | expr5 PLUS expr_unary { std::cerr << "add" << std::endl ; }
        | expr5 MINUS expr_unary { std::cerr << "sub" << std::endl ; }

expr_unary  : PLUS expr6
            | MINUS expr6 { std::cerr << "neg" << std::endl; }
            | expr6

expr6   : expr7
        | expr6 mult_op expr7 { std::cerr << "mult_op" << std::endl; }

expr7   : expr8
        | FLOOR LPAREN expr RPAREN
        | LENGTH LPAREN expr RPAREN
        | SUBSTR LPAREN expr COMMA expr COMMA expr RPAREN
        | CHARACTER LPAREN expr RPAREN
        | NUMBERSYM LPAREN expr RPAREN
        | FIX LPAREN expr RPAREN
        | FLOAT LPAREN expr RPAREN

expr8   : variable
        | constant
        | LPAREN expr RPAREN { $$ = $2; }
        | function_call

relation_op : LSS
            | GTR
            | EQL
            | NEQ
            | LEQ
            | GEQ

mult_op     : TIMES
            | SLASH
            | MOD

constant    : NUMBER { $$ = $1; }
            | TRUE
            | FALSE
            | TEXT

function_call : function_ident LPAREN RPAREN { printf("function call()\n"); }
              | function_ident actual_params

function_ident   : IDENT

actual_params   : actual_params_header RPAREN

actual_params_header : LPAREN expr
                     | actual_params_header COMMA expr

variable    : IDENT { $$ = make_ident($1); }
            | variable PERIOD IDENT
            | variable LBRACK expr RBRACK

%%
/* -------------- body section -------------- */
// feel free to add your own C/C++ code here

// extern 
extern int yylineno;
void yyerror(const char *s) {
    fprintf(stderr, " line %d: %s\n", yylineno, s);
}

TreeNode *make_ident(TreeNode *p1)
{
    return p1;
}
