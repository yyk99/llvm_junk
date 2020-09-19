//
// parser_bits.h
//

#ifndef __PARSER_BITS_H
#define __PARSER_BITS_H

#include "TreeNode.h"

#include <llvm/ADT/ArrayRef.h>
#include <vector>

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
    class Value;
    class Type;
    class StructType;
    class LLVMContext;
}

llvm::LLVMContext *get_global_context();

typedef llvm::ArrayRef<llvm::Type*> TypeArray;

llvm::Function *get_current_function();

//
// prototypes
//

llvm::Value *generate_expr(TreeNode *expr);
llvm::Value *generate_load(TreeIdentNode *node);
llvm::Value *generate_rtl_call(const char *entry, std::vector<llvm::Value *> const &args);

llvm::Type *CreateArrayType(llvm::Type *item, size_t ndim = 1);
llvm::Type *CreateStructType(llvm::Type *item, size_t n);
llvm::StructType *array_get_type(llvm::Value *sym);
llvm::Type *array_get_elem_type(llvm::StructType *arr_type);
llvm::Value *generate_alloca(TreeNode *type_node, std::string const &name);
llvm::Value *generate_dot(TreeNode *type_node);

llvm::Value *Const(int c);

// TODO: class?
bool symbols_insert(std::string const &s, llvm::Value *v);
llvm::Value * symbols_find(std::string const &s);
llvm::Value * symbols_find_function(std::string const &s);
bool symbols_insert_function(std::string const &s, llvm::Function *v);

void symbols_push();
void symbols_pop();
bool isArrayType(llvm::Value *sym);

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

llvm::Type *NodeToType(TreeNode *type);

typedef std::pair<llvm::Type *, llvm::Value *> type_value_t;

type_value_t create_alloca(llvm::Type *t, const char *s);
type_value_t NodeToType(TreeNode *node, const char *sym);

extern bool flag_verbose;

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
#endif
