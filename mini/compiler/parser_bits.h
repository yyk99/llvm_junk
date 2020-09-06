//
// parser_bits.h
//

#ifndef __PARSER_BITS_H
#define __PARSER_BITS_H

#include "TreeNode.h"

void program_header(TreeNode *);
void program_end(TreeNode *);
void init_compiler();
TreeNode *make_binary(TreeNode *, TreeNode *, int op);
TreeNode *make_unary(TreeNode *, int op);
TreeNode *make_boolean(int op);
void assign_statement(TreeNode *targets, TreeNode *expr);
TreeNode *base_type(int type);
void variable_declaration(TreeNode *variables, TreeNode *type);
TreeNode *make_output(TreeNode *tree, bool append_nl = false);

void cond_specification(TreeNode *);

namespace llvm {
    class Function;
}
llvm::Function *get_current_function();

void false_branch_begin();
void false_branch_end();
void true_branch_end();
void simple_cond_statement();
void loop_head(TreeNode *loop_target, TreeNode *control);
TreeNode *control(TreeNode *step_control, TreeNode *cond_control = 0);
void loop_footer(TreeNode *ident = 0);
void set_label(TreeNode *);
void set_for_label(TreeNode *);
void clear_label();
void make_repent(TreeNode *);
void make_repeat(TreeNode *);


void function_header(TreeNode *);

void function_end(TreeNode *);
void subroutine_end(TreeNode *);

void return_statement();
void return_statement(TreeNode *);

TreeNode *type_identifier(TreeNode *);

void type_declaration(TreeNode *ident, TreeNode *type);
    
extern bool flag_verbose;

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
#endif
