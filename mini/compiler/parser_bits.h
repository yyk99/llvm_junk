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

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
#endif