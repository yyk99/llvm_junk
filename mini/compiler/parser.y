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
  TreeNode *node;
}

%token <num> PROGRAMSYM COLON RETURNSYM
%token <num> WHILE ELSESYM REPENT REPEAT BY TO FOR SELECT OF OTHERWISE CASE
%token <num> SETSYM FISYM EXITSYM INPUT OUTPUT XOR
%token <num> TIMES SLASH LPAREN RPAREN SEMICOLON COMMA PERIOD BECOMES EQL NEQ LSS GTR
%token <num> LEQ GEQ BEGINSYM CALLSYM CONSTSYM DOSYM
%token <num> ENDSYM IFSYM PROCSYM THENSYM VARSYM
%token <node> IDENT
%token <node> NUMBER
%token <num> UNKNOWN LBRACK RBRACK
%token <num> PLUS INTEGER MINUS AND OR NOT
%token <num> CONCAT /* || */
%token <num> FLOOR LENGTH SUBSTR CHARACTER NUMBERSYM FIX MOD TRUE FALSE FLOAT
%token <node> TEXT
%token <num> T_TYPE
%token <num> T_IS
%token <num> T_INTEGER
%token <num> T_REAL
%token <num> T_BOOLEAN
%token <num> T_STRING
%token <num> T_DECLARE T_PROCEDURE T_FUNCTION

%type <node> proc_declaration variable_declaration
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
%type <num> relation_op 
%type <num> mult_op     
%type <node> constant    
%type <node> function_call 
%type <node> function_ident   
%type <node> actual_params   
%type <node> actual_params_header 
%type <node> variable    
%type <num> base_type
%type <node> type
%type <node> declared_names_list
%type <node> declared_names
%type <node> type_declarations
%type <node> type_declaration

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
segment_body    : type_declarations
                variable_declarations
                proc_declarations executable_statements

type_declarations : {} 
                  | type_declarations type_declaration

variable_declarations : 
                      | variable_declarations variable_declaration 

proc_declarations     : {}
                      | proc_declarations proc_declaration {}

type_declaration      : T_TYPE IDENT T_IS type {}

type                  : base_type { $$ = base_type($1); }
/* TODO: array,....etc */

base_type              : T_INTEGER  { $$ = T_INTEGER; } 
                       | T_REAL     { $$ = T_REAL; }
                       | T_BOOLEAN  { $$ = T_BOOLEAN; }
                       | T_STRING   { $$ = T_STRING; }

variable_declaration   : T_DECLARE declared_names type SEMICOLON { variable_declaration($2, $3); }

declared_names         : IDENT { $$ = make_ident($1); }
                       | declared_names_list RPAREN { $$ = $1; }

declared_names_list    : LPAREN IDENT { $$ = $2; }
                       | declared_names_list COMMA IDENT { $$ = make_binary($1, $3, COMMA); }


proc_declaration       : T_PROCEDURE IDENT SEMICOLON {}
                       | T_FUNCTION IDENT SEMICOLON {}


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

empty_statement         : SEMICOLON {}

repeat_statement        : REPEAT IDENT SEMICOLON { make_repeat($2); }

repent_statement        : REPENT IDENT SEMICOLON { make_repent($2); }

compound_statement      : simple_compound_statement {}
                        | label { set_label($1); } simple_compound_statement { clear_label(); }

simple_compound_statement : compound_header compound_body compound_footer

compound_header         : BEGINSYM {}

compound_body           : segment_body { $$ = $1; }

compound_footer         : ENDSYM SEMICOLON {}
                        | ENDSYM IDENT SEMICOLON { $$ = make_ident($2); }

call_statement          : CALLSYM IDENT SEMICOLON {}

exit_statement          : EXITSYM SEMICOLON {}

conditional_statement   : simple_cond_statement
                        | label { set_label($1); } simple_cond_statement { clear_label(); }

simple_cond_statement   : cond_specification true_branch FISYM SEMICOLON { true_branch_end(); }
                        | cond_specification true_branch false_branch FISYM SEMICOLON 

cond_specification      : IFSYM expr { cond_specification($2); }

true_branch             : THENSYM cond_statement_body {}

false_branch            : ELSESYM { false_branch_begin(); } cond_statement_body { false_branch_end(); }

cond_statement_body     : segment_body


/* select */
select_statement        : simple_select_statement
                        | label { set_label($1); } simple_select_statement { clear_label(); }
simple_select_statement : select_header select_body select_footer
select_header           : SELECT expr OF { $$ = $2; }
select_body             : case_list
                        | case_list other_cases
select_footer           : ENDSYM SELECT SEMICOLON {}
                        | ENDSYM SELECT IDENT SEMICOLON { $$ = make_ident($3); }
case_list               : case
                        | case case_list
case                    : case_head case_body
                         
case_head               : CASE selector COLON { $$ = $2; }
selector                : selector_head
                        | selector_head selector

selector_head           : expr { $$ = $1; }

other_cases             : other_header case_body

other_header            : OTHERWISE COLON {}

case_body               : segment_body { $$ = $1; }

return_statement        : RETURNSYM SEMICOLON {}
                        | RETURNSYM expr SEMICOLON { $$ = $2; }

assign_statement        : SETSYM target_list expr SEMICOLON { assign_statement($2, $3); }

target_list             : target { $$ = $1; }
                        | target_list target { $$ = make_binary($1, $2, BECOMES); } 

target                  : variable BECOMES { $$ = $1; }


/* loop */
loop_statement          : simple_loop_statement
                        | label { set_for_label($1); } simple_loop_statement { clear_label(); }

simple_loop_statement   : loop_head loop_body loop_footer

loop_head               : for loop_target control DOSYM { loop_head($2, $3); }

loop_body               : segment_body

loop_footer             : ENDSYM FOR SEMICOLON { loop_footer() ;}
                        | ENDSYM FOR IDENT SEMICOLON { loop_footer($3); }

for                     : FOR

loop_target             : variable BECOMES { $$ = $1; }

control                 : step_control { $$ = control ($1); }
                        | step_control cond_control { $$ = control ($1, $2); }

step_control            : start_value step { $$ = make_binary($1, make_binary($2, 0, BY), TO); }
                        | start_value limit { $$ = make_binary($1, make_binary(0, $2, BY), TO); }
                        | start_value step limit { $$ = make_binary($1, make_binary($2, $3, BY), TO); }

start_value             : expr { $$ = $1; }
step                    : BY expr { $$ = $2; }
limit                   : TO expr { $$ = $2; }
cond_control            : WHILE expr { $$ = $2; }

input_statement         : INPUT input_list {}
input_list              : variable { $$ = $1; }
                        | variable COMMA input_list { $$ = $1; }


output_statement        : OUTPUT output_list { $$ = make_output($2, true); }

output_list             : expr { $$ = $1; }
                        | expr COMMA output_list { $$ = make_binary($1, $3, COMMA); }
                        
label                   : IDENT COLON { $$ = $1; }

/* expr */
expr                    : expr1 { $$ = $1; }
                        | expr1 OR expr { $$ = make_binary ($1, $3, OR); }
                        | expr1 XOR expr { $$ = make_binary ($1, $3, XOR); }

expr1                   : expr2 { $$ = $1; }
                        | expr2 AND expr1 {  $$ = make_binary ($1, $3, AND); }

expr2                   : expr3 { $$ = $1; }
                        | NOT expr3 { $$ = make_unary ($2, NOT);}

expr3                   : expr4 { $$ = $1; }
                        | expr4 relation_op expr3 { $$ = make_binary ($1, $3, $2); }

expr4                   : expr5 { $$ = $1; }
                        | expr5 CONCAT expr4 { $$ = make_binary ($1, $3, CONCAT); }

expr5   : expr_unary { $$ = $1; }
        | expr5 PLUS expr_unary { $$ = make_binary($1, $3, PLUS); }
        | expr5 MINUS expr_unary { $$ = make_binary($1, $3, MINUS); }

expr_unary  : PLUS expr6 { $$ = $2; }
            | MINUS expr6 { $$ = make_unary($2, MINUS); }
            | expr6 {$$ =  $1;}

expr6   : expr7  {$$ =  $1;}
        | expr6 mult_op expr7 { $$ = make_binary($1, $3, $2); }

expr7   : expr8 {$$ =  $1;}
        | FLOOR LPAREN expr RPAREN { $$ = make_unary($3, $1); }
        | LENGTH LPAREN expr RPAREN { $$ = make_unary($3, $1); }
        | SUBSTR LPAREN expr COMMA expr COMMA expr RPAREN { $$ = make_binary($3, $5, $1); }
        | CHARACTER LPAREN expr RPAREN { $$ = make_unary($3, $1); }
        | NUMBERSYM LPAREN expr RPAREN { $$ = make_unary($3, $1); }
        | FIX LPAREN expr RPAREN { $$ = make_unary($3, $1); }
        | FLOAT LPAREN expr RPAREN { $$ = make_unary($3, $1); }

expr8   : variable { $$ = $1; }
        | constant { $$ = $1; }
        | LPAREN expr RPAREN { $$ = $2; }
        | function_call { $$ = $1; }

relation_op : LSS { $$ = LSS; } 
            | GTR { $$ = GTR; }
            | EQL { $$ = EQL; }
            | NEQ { $$ = NEQ; }
            | LEQ { $$ = LEQ; }
            | GEQ { $$ = GEQ; }

mult_op     : TIMES { $$ = TIMES; }
            | SLASH { $$ = SLASH; }
            | MOD { $$ = MOD; }

constant    : NUMBER { $$ = $1; }
            | TRUE { $$ = make_boolean(1) ; }
            | FALSE { $$ = make_boolean(0); }
            | TEXT { $$ = $1; }

function_call : function_ident LPAREN RPAREN { $$ = $1; }
              | function_ident actual_params { $$ = make_binary($1, $2, CALLSYM); }

function_ident   : IDENT { $$ = $1; }

actual_params   : actual_params_header RPAREN { $$ = $1; }

actual_params_header : LPAREN expr { $$ = $2; }
                     | actual_params_header COMMA expr { $$ = make_binary ($1, $3, COMMA); }

variable    : IDENT { $$ = make_ident($1); }
            | variable PERIOD IDENT { $$ = make_binary ($1, $3, PERIOD); }
            | variable LBRACK expr RBRACK { $$ = make_binary ($1, $3, LBRACK); }

%%
/* -------------- body section -------------- */
// feel free to add your own C/C++ code here

// extern 
extern int yylineno;
extern int err_cnt;
void yyerror(const char *s) {
    ++err_cnt;
    fprintf(stderr, " line %d: %s\n", yylineno, s);
}

TreeNode *make_ident(TreeNode *p1)
{
    return p1;
}

// Local Variables:
// mode: text
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
