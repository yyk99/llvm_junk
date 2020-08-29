//
//
//

#include "parser.h"
#include "parser_bits.h"
#include "TreeNode.h"

// #include <iostream>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <algorithm>
#include <cstdlib>
#include <map>
#include <stack>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

using namespace llvm;

typedef SmallVector<BasicBlock *, 16> BBList;
typedef SmallVector<Value *, 16> ValList;

static llvm::LLVMContext TheContext;

class IfStatement {
    BasicBlock *createBB(Function *f, std::string const &name)
    {
        return BasicBlock::Create(TheContext, name, f);
    }

public:
    BasicBlock *ThenBB; // = createBB(fooFunc, "then");
    BasicBlock *ElseBB; // = createBB(fooFunc, "else");
    BasicBlock *MergeBB; // = createBB(fooFunc, "ifcont");

    IfStatement() : ThenBB(0),ElseBB(0),MergeBB(0) {}
    IfStatement(Function *f)
        : ThenBB(createBB(f, "then"))
        , ElseBB(createBB(f, "else"))
        , MergeBB(createBB(f, "ifcont")) {}
};

class LoopStatement {
public:
    TreeNode *Target;
    Value *By;
    Value *To;
    
    LoopStatement() : Target(0), By(0), To(0) {};
    LoopStatement(TreeNode *target, Value *by, Value *to)
        : Target(target)
        , By(by)
        , To(to)
    {
    }
};

//
// prototypes
//
Value *generate_load(TreeIdentNode *node);

std::unordered_map<std::string, llvm::Value *> symbols;
std::unordered_map<std::string, llvm::Function *> rtl_symbols;

static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

static std::stack<Function *> functions;

static std::stack<IfStatement> conditionals;
static std::stack<LoopStatement> loops;

int err_cnt = 0;
bool flag_verbose = false;

//
//
//
void init_compiler()
{
    // TODO: place init code here
}

///
///
///
void insert_rtl_symbol(std::string const &key_name, std::string const &entry_name,
                       Type *return_type, std::vector<Type *> const &formals)
{
    FunctionType *FT = FunctionType::get(return_type, formals, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, entry_name, TheModule.get());
    unsigned idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(std::string("arg_") + std::to_string(++idx));

    rtl_symbols.insert(std::make_pair(key_name, F));
}

//
//
//
void init_rtl_symbols()
{
    insert_rtl_symbol("output", "rtl_output", Type::getInt32Ty(TheContext), {Type::getInt32Ty(TheContext)});
    insert_rtl_symbol("output_str", "rtl_output_str", Type::getInt32Ty(TheContext), {Type::getInt8PtrTy(TheContext)});
    insert_rtl_symbol("output_real", "rtl_output_real", Type::getInt32Ty(TheContext), {Type::getDoubleTy(TheContext)});
    insert_rtl_symbol("output_nl", "rtl_output_nl", Type::getInt32Ty(TheContext), {});
}

//
// process main programm
//

//llvm::Function *F;
void program_header(TreeNode *node)
{
    auto id = dynamic_cast<TreeIdentNode *>(node);

    TheModule = llvm::make_unique<llvm::Module>(id->id, TheContext);

    init_rtl_symbols();
    
    std::vector<llvm::Type *> Doubles(0, llvm::Type::getDoubleTy(TheContext));
    FunctionType *FT = FunctionType::get(Builder.getInt32Ty(), Doubles, false);
    Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", TheModule.get());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName("arg");

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);

    functions.push(F);
}

void program_end(TreeNode *node)
{
    auto F = get_current_function();
    // TODO: pop(); ... ; delete F;
    
    auto rc = Builder.getInt32(0);

    Builder.CreateRet(rc);

    verifyFunction(*F);

    // auto id = dynamic_cast<TreeIdentNode *>(node);
    // TODO: verify ending label == module name

    if(err_cnt == 0)
        TheModule->print(outs(), nullptr);
}

TreeNode *make_binary(TreeNode *left, TreeNode *right, int op)
{
    return new TreeBinaryNode(left, right, op);
}

TreeNode *make_unary(TreeNode *left, int op)
{
    return new TreeUnaryNode(left, op); // TODO:
}

TreeNode *make_boolean(int op)
{
    return new TreeBooleanNode(op != 0);
}

void syntax_error(std::string errmsg)
{
    ++err_cnt;
    errs() << errmsg << "\n";
}

void generate_store(TreeNode *targets, Value *e)
{
    if(auto node = dynamic_cast<TreeIdentNode *>(targets)) {
        std::string id = node->id;
        auto pos = symbols.find(id);
        if(pos != symbols.end()) {
            Builder.CreateStore(e, pos->second);
        } else {
            syntax_error(id + ": ident not found");
        }
    } else if (auto node = dynamic_cast<TreeBinaryNode *>(targets)) {
        generate_store(node->left, e);
        generate_store(node->right, e);
    } else {
        errs() << "generate_store: " << typeid(*targets).name() << '\n';
        // TODO: report error?
    }
}

Value *generate_load(TreeIdentNode *node)
{
    Value *val = 0;
    std::string id = node->id;
    auto pos = symbols.find(id);
    if(pos != symbols.end()) {
#if 1
        val = Builder.CreateLoad(pos->second, "tmpvar");
#else
        val = pos->second;
#endif
    } else {
        syntax_error(id + ": ident not found");
    }
    return val;
}

Value *allocate_string_constant(TreeTextNode *node)
{
    Value *val = Builder.CreateGlobalStringPtr(node->text, "c_str");
    return val;
}

//  L > R
Value *generate_compare_gtr_expr(Value *L, Value *R)
{
    Value *val;
    if(L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
        val = Builder.CreateFCmpUGT(L, R, "cmptmp");
    else
        val = Builder.CreateICmpSGT(L, R, "cmptmp");
    return val;
}

// L = R
Value *generate_compare_eql_expr(Value *L, Value *R)
{
    Value *val;
    if(L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
        val = Builder.CreateFCmpUEQ(L, R, "cmptmp");
    else
        val = Builder.CreateICmpEQ(L, R, "cmptmp");
    return val;
}

Value *generate_expr(TreeNode *expr)
{
    Value *val = 0;

    if(flag_verbose)
        errs() << "generate_expr: " << typeid(*expr).name() << '\n';
    
    if(auto bp = dynamic_cast<TreeBinaryNode *>(expr)) {
        Value *L = generate_expr(expr->left);
        Value *R = generate_expr(expr->right);
        if(bp->oper == PLUS)
            val = Builder.CreateAdd(L, R, "addtmp");
        else if(bp->oper == MINUS)
            val = Builder.CreateSub(L, R, "subtmp");
        else if(bp->oper == TIMES)
            val = Builder.CreateMul(L, R, "multmp");
        else if(bp->oper == SLASH)
            val = Builder.CreateMul(L, R, "divtmp");
        else if(bp->oper == GTR)
            val = generate_compare_gtr_expr(L, R);
        else if(bp->oper == EQL)
            val = generate_compare_eql_expr(L, R);
        else
            llvm::errs() << "Not implemented op: " << bp->oper << "\n";
    } else if (auto up = dynamic_cast<TreeUnaryNode *>(expr)) {
    } else if (auto np = dynamic_cast<TreeNumericalNode *>(expr)) {
        val = ConstantInt::get(Type::getInt32Ty(TheContext), np->num);
    } else if (auto np = dynamic_cast<TreeDNumericalNode *>(expr)) {
        val = ConstantFP::get(Type::getDoubleTy(TheContext), np->num);
    } else if (auto node = dynamic_cast<TreeIdentNode *>(expr)) {
        val = generate_load(node);
    } else if (auto node = dynamic_cast<TreeTextNode *>(expr)) {
        val = allocate_string_constant(node);
    } else {
        errs() << "Hm...\n"; 
    }

    return val;
}

void assign_statement(TreeNode *targets, TreeNode *expr)
{
    if(flag_verbose)
        errs() << targets->show() << " = " << expr->show() << "\n";

    Value *e = generate_expr(expr);
    generate_store(targets, e);
}

TreeNode *base_type(int type)
{
    return make_unary(0, type); // TODO: ....
}

void get_ids(TreeNode *vars, std::vector<std::string> &res)
{
    if(vars == 0)
        return;
    if(auto bn = dynamic_cast<TreeBinaryNode *>(vars)) {
        get_ids(bn->left, res);
        get_ids(bn->right, res);
    } else if (auto id = dynamic_cast<TreeIdentNode *>(vars)) {
        res.push_back(id->id);
    }
}

// TODO: implement
Type *NodeToType(TreeNode *type)
{
    if(auto node = dynamic_cast<TreeUnaryNode *>(type)) {
        if(node->oper == T_STRING)
            return Type::getInt8PtrTy(TheContext);
        if(node->oper == T_REAL)
            return Type::getDoubleTy(TheContext);
    }
    return Type::getInt32Ty(TheContext);
}

void variable_declaration(TreeNode *variables, TreeNode *type)
{
    std::vector<std::string> names;
    get_ids(variables, names);
    for(auto s : names) {
        auto symb = Builder.CreateAlloca(NodeToType(type), 0, s.c_str());
        auto res = symbols.insert(std::make_pair(s, symb));
    }
}

void generate_rtl_call(const char *entry, std::vector<Value *> const &args)
{
    auto pos = rtl_symbols.find(entry);
    if(pos != rtl_symbols.end()) {
        if(Function *function = dynamic_cast<Function*>(pos->second)){
            Builder.CreateCall(function, args, "calltmp");
        }
    } else {
        ++err_cnt;
        errs() << "generate_rtl_call: " << entry << " not defined\n";
    }
}

void generate_output_call(std::vector<Value *> const &val)
{
    if(val[0]->getType() == Type::getInt8PtrTy(TheContext))
        generate_rtl_call("output_str", val);
    else if (val[0]->getType() == Type::getDoubleTy(TheContext))
        generate_rtl_call("output_real", val);
    else
        generate_rtl_call("output", val);
}

TreeNode *make_output(TreeNode *expr, bool append_nl)
{
    //  make_output: 14TreeBinaryNode
    //  make_output: 17TreeNumericalNode
    //  make_output: 13TreeIdentNode

    if(auto node = dynamic_cast<TreeBinaryNode *>(expr)) {
        if(node->oper == COMMA) {
            std::vector<Value *> val {generate_expr(node->left)};
            generate_output_call(val);
            make_output(node->right);
        } else {
            std::vector<Value *> val {generate_expr(node)};
            generate_output_call(val); 
        }
    } else {
        std::vector<Value *> val {generate_expr(expr)};
        generate_output_call(val);
    }

    if(append_nl) {
        std::vector<Value *> val(0); //no args
        generate_rtl_call("output_nl", val);
    }
    
    return 0;
}

Function * get_current_function()
{
    return functions.top();
}

void cond_specification(TreeNode *expr)
{
    auto if_stat = IfStatement(get_current_function());

    conditionals.push(if_stat);
    Value *Compare = generate_expr(expr);

    Value *Condtn;
    if(Compare->getType() == Type::getInt1Ty(TheContext)) {
        Value *Zero = Builder.getInt1(false);
        Condtn = Builder.CreateICmpNE(Compare, Zero, "ifcond");
#if 0
    } else if(Compare->getType() == Type::getInt32Ty(TheContext)) {
        Value *Zero = Builder.getInt32(0);
        Condtn = Builder.CreateICmpNE(Compare, Zero, "ifcond");
#endif
    } else {
        syntax_error("Must be boolean type");
    }

    Builder.CreateCondBr(Condtn, if_stat.ThenBB, if_stat.ElseBB);
    Builder.SetInsertPoint(if_stat.ThenBB);
}

void false_branch_begin()
{
    auto &cond = conditionals.top();
    Builder.CreateBr(cond.MergeBB);
    Builder.SetInsertPoint(cond.ElseBB);
}

void false_branch_end()
{
    auto &cond = conditionals.top();

    Builder.CreateBr(cond.MergeBB);
    Builder.SetInsertPoint(cond.MergeBB);

    simple_cond_statement();
}

// if <cond> then <true-branch> fi;
void true_branch_end()
{
    auto &cond = conditionals.top();
    Builder.CreateBr(cond.ElseBB);
    Builder.SetInsertPoint(cond.ElseBB);
    Builder.CreateBr(cond.MergeBB);
    Builder.SetInsertPoint(cond.MergeBB);
    
    simple_cond_statement();
}

void simple_cond_statement()
{
    conditionals.pop();
}

//
// E.g.
//     loop_target: i
//     control: FOR(TO(1 BY(<null> 10)) <null>)
//
//   MergeBB:
//           if (index > to) goto ElseBB;
//           <loop-body>
//           index = index + by;
//           goto MergeBB;
//   ElseBB:
//           
//
void loop_head(TreeNode *loop_target, TreeNode *control)
{
    if(flag_verbose) {
        errs() << "loop_target: " << loop_target->show() << "\n";
        errs() << "control: " << control->show() << "\n";
    }
    
    if(auto for_node = dynamic_cast<TreeBinaryNode *>(control)) {
        Value *expr_step = 0;
        Value *expr_to = 0;
        if(for_node->oper == FOR) {
            if(auto to_node = dynamic_cast<TreeBinaryNode *>(for_node->left)) {
                Value *init_expr = generate_expr(to_node->left);
                generate_store(loop_target, init_expr);

                if(auto by_node = dynamic_cast<TreeBinaryNode *>(to_node->right)) {
                    // by_node->oper == BY
                    expr_step = by_node->left ? generate_expr(by_node->left) : Builder.getInt32(1);
                    expr_to = by_node->right ? generate_expr(by_node->right) : 0;
                }
                
                auto if_stat = IfStatement(get_current_function());
                conditionals.push(if_stat);

                auto loop_stat = LoopStatement(loop_target, expr_step, expr_to);
                loops.push(loop_stat);

                Builder.CreateBr(if_stat.MergeBB);
                Builder.SetInsertPoint(if_stat.MergeBB);
                Value *index = generate_load(dynamic_cast<TreeIdentNode *>(loop_target));
                if(expr_to) {
                    Value *cmp = Builder.CreateICmpSLE(index, expr_to, "cmp");
                    Value *Zero = Builder.getInt1(false);
                    Value *ifcond = Builder.CreateICmpNE(cmp, Zero, "ifcond");
                    Builder.CreateCondBr(ifcond, if_stat.ThenBB, if_stat.ElseBB);
                    Builder.SetInsertPoint(if_stat.ThenBB);
                }
            }
            if(auto cond_control = for_node->right) {
                // Generate "while"
                errs() << "while......\n";
            }
        } else {
            syntax_error("Unexpected operation = " + std::to_string(for_node->oper));
        }
    } else {
        // Never here
    }
}

//
// cond_control (optional)
//
TreeNode *control(TreeNode *step_control, TreeNode *cond_control)
{
    return make_binary(step_control, cond_control, FOR);
}

//
// ident (optional)
//
void loop_footer(TreeNode *ident)
{
    auto &cond = conditionals.top();
    auto &loop = loops.top();

    // TODO:  index += step;

    Value *index = generate_load(dynamic_cast<TreeIdentNode *>(loop.Target));
    index = Builder.CreateAdd(index, loop.By, "increment");
    generate_store(loop.Target, index);
    Builder.CreateBr(cond.MergeBB);
    
    //    Builder.CreateBr(cond.ElseBB);
    Builder.SetInsertPoint(cond.ElseBB);
    
    conditionals.pop();
    loops.pop();
}


void set_label(TreeNode *label)
{
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
