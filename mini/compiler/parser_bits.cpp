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
#include "llvm/IR/Argument.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <algorithm>
#include <cstdlib>
#include <map>
#include <stack>
#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

using namespace llvm;

typedef SmallVector<BasicBlock *, 16> BBList;
typedef SmallVector<Value *, 16> ValList;

static LLVMContext TheContext;

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
    TreeNode *By;
    TreeNode *To;
    
    LoopStatement() : Target(0), By(0), To(0) {};
    LoopStatement(TreeNode *target, TreeNode *by, TreeNode *to)
        : Target(target)
        , By(by)
        , To(to)
    {
    }
};

class LabelStatement {
    BasicBlock *createBB(Function *f, std::string const &name)
    {
        return BasicBlock::Create(TheContext, name, f);
    }

    Function *f;
public:
    BasicBlock *RepeatBB;
    BasicBlock *RepentBB;
    std::string label; 
    
    LabelStatement()
        : RepeatBB{}
        , RepentBB{}
        , label{}
        , f{}
    {}

    LabelStatement(Function *f, std::string const &l)
        : RepeatBB{createBB(f, "bb")}
        , RepentBB{}
        , label(l)
        , f(f)
    {
    }

    // Label does not have branches. The branches willbe set from the
    // labeled block (e.g. for-loop) later
    LabelStatement(std::string const &l)
        : RepeatBB{}
        , RepentBB{}
        , label(l)
        , f()
    {
    }

    // the method will be used to create a label "on-demand"
    BasicBlock *getRepentBB() {
        if (RepentBB == 0 && f)
            RepentBB = createBB(f, "be");
        return RepentBB;
    }

    bool isForLoop() const
    {
        return f == 0;
    }
};

static std::stack<LabelStatement *> labels;
static std::unordered_map<std::string, LabelStatement *> label_table;

//
// prototypes
//
Value *generate_expr(TreeNode *expr);
Value *generate_load(TreeIdentNode *node);
Value *generate_rtl_call(const char *entry, std::vector<Value *> const &args);

std::unordered_map<std::string, Value *> symbols;
std::unordered_map<std::string, Function *> rtl_symbols;

bool insert_symbol(std::string const &s, Value *v);


static IRBuilder<> Builder(TheContext);
static std::stack<Module *> modules;
static Module  *TheModule()
{
    return modules.top();
}

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
    Function *F = Function::Create(FT, Function::ExternalLinkage, entry_name, TheModule());
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
    insert_rtl_symbol("output_bool", "rtl_output_bool", Type::getInt1Ty(TheContext), {Type::getInt1Ty(TheContext)});
    insert_rtl_symbol("output_nl", "rtl_output_nl", Type::getInt32Ty(TheContext), {});
    insert_rtl_symbol("fix", "rtl_fix", Type::getInt32Ty(TheContext), {Type::getDoubleTy(TheContext)});
}

//
// process main programm
//

void program_header(TreeNode *node)
{
    auto id = dynamic_cast<TreeIdentNode *>(node);

    modules.push(new Module(id->id, TheContext));

    init_rtl_symbols();
    
    std::vector<Type *> Doubles(0, Type::getDoubleTy(TheContext));
    FunctionType *FT = FunctionType::get(Builder.getInt32Ty(), Doubles, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, "main", TheModule());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName("arg");

    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", F);
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
        TheModule()->print(outs(), nullptr);
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

//
bool is_loadable(Value *val, std::string id)
{
    return !(val->getValueID() == Value::ArgumentVal);
}

Value *generate_load(TreeIdentNode *node)
{
    Value *val = 0;
    std::string id = node->id;
    auto pos = symbols.find(id);
    if(pos != symbols.end()) {
        if(is_loadable(pos->second, id)) {
            val = Builder.CreateLoad(pos->second, "tmpvar");
        } else {
            val = pos->second;
        }
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
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");
    
    Value *val;
    if(L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
        val = Builder.CreateFCmpUGT(L, R, "gtr");
    else
        val = Builder.CreateICmpSGT(L, R, "gtr");
    return val;
}

//  L >= R
Value *generate_compare_geq_expr(Value *L, Value *R)
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");

    Value *val;
    if(L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
        val = Builder.CreateFCmpUGE(L, R, "geq");
    else
        val = Builder.CreateICmpSGE(L, R, "geq");
    return val;
}

//  L < R
Value *generate_compare_lss_expr(Value *L, Value *R)
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");

    Value *val;
    if(L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
        val = Builder.CreateFCmpULT(L, R, "cmptmp");
    else
        val = Builder.CreateICmpSLT(L, R, "cmptmp");
    return val;
}

//  L <= R
Value *generate_compare_leq_expr(Value *L, Value *R)
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");

    Value *val;
    if(L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
        val = Builder.CreateFCmpULE(L, R, "leq");
    else
        val = Builder.CreateICmpSLE(L, R, "leq");
    return val;
}

// L = R
Value *generate_compare_eql_expr(Value *L, Value *R)
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");

    Value *val;
    if(L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
        val = Builder.CreateFCmpUEQ(L, R, "cmptmp");
    else
        val = Builder.CreateICmpEQ(L, R, "cmptmp");
    return val;
}

Value *generate_div(Value *L, Value *R)
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");
    
    Value *val = 0;
    if(L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
        val = Builder.CreateFDiv(L, R, "fdiv");
    else
        val = Builder.CreateSDiv(L, R, "div");

    return val;
}

Value *generate_sub(Value *L, Value *R)
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");
    
    Value *val = 0;
    if(L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
        val = Builder.CreateFSub(L, R, "fsub");
    else
        val = Builder.CreateSub(L, R, "sub");

    return val;
}

Value *generate_mul(Value *L, Value *R)
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");
    
    Value *val = 0;
    if(L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
        val = Builder.CreateFMul(L, R, "fmul");
    else
        val = Builder.CreateMul(L, R, "mul");

    return val;
}

Value *generate_add(Value *L, Value *R, const char *name = "add")
{
    if (L->getType()->isDoubleTy() && R->getType()->isIntegerTy())
        R = Builder.CreateSIToFP(R, Type::getDoubleTy(TheContext), "float");
    else if (L->getType()->isIntegerTy() && R->getType()->isDoubleTy())
        L = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");

    Value *val;
    if(L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
        val = Builder.CreateFAdd(L, R, name);
    else
        val = Builder.CreateAdd(L, R, name);
    return val;
}

void build_actual_args(TreeNode *anode, std::vector<Value *> &args)
{
    if(auto bnode = dynamic_cast<TreeBinaryNode*>(anode)) {
        assert(bnode->oper == COMMA);
        build_actual_args(bnode->left, args);
        build_actual_args(bnode->right, args);
    } else {
        args.push_back(generate_expr(anode));
    }
}

Value *generate_call(TreeNode *fnode, TreeNode *anode)
{
    Value *val = 0;

    if(auto ident = dynamic_cast<TreeIdentNode *>(fnode)) {
        auto pos = symbols.find(ident->id);
        if(pos != symbols.end()) {
            Value *F = pos->second;
            std::vector<Value *> args;
            build_actual_args(anode, args);
            val = Builder.CreateCall(F, args, "fcall");   
        }
    } else {
        syntax_error("Function name must be ident");
    }

    return val;
}

Value *generate_expr(TreeNode *expr)
{
    Value *val = 0;

    if(flag_verbose)
        errs() << "generate_expr: " << typeid(*expr).name() << '\n';
    
    if(auto bp = dynamic_cast<TreeBinaryNode *>(expr)) {
        if(bp->oper == CALLSYM)
            val = generate_call(expr->left, expr->right);
        else {            
            Value *L = generate_expr(expr->left);
            Value *R = generate_expr(expr->right);
            if(bp->oper == PLUS)
                val = generate_add(L, R);
            else if(bp->oper == MINUS)
                val = generate_sub(L, R);
            else if(bp->oper == TIMES)
                val = generate_mul(L, R);
            else if(bp->oper == SLASH)
                val = generate_div(L, R);
            else if(bp->oper == GTR)
                val = generate_compare_gtr_expr(L, R);
            else if(bp->oper == LEQ)
                val = generate_compare_leq_expr(L, R);
            else if(bp->oper == LSS)
                val = generate_compare_lss_expr(L, R);
            else if(bp->oper == GEQ)
                val = generate_compare_geq_expr(L, R);
            else if(bp->oper == EQL)
                val = generate_compare_eql_expr(L, R);
            else if(bp->oper == AND)
                val = Builder.CreateAnd(L, R, "andtmp");
            else if(bp->oper == OR)
                val = Builder.CreateOr(L, R, "ortmp");
            else
                errs() << "Not implemented op: " << bp->oper << "\n";
        }
    } else if (auto up = dynamic_cast<TreeUnaryNode *>(expr)) {
        Value *L = generate_expr(expr->left);
        if (up->oper == MINUS) {
            if(L->getType()->isFloatingPointTy())
                val = Builder.CreateFNeg(L, "fneg");
            else
                val = Builder.CreateNeg(L, "neg");
        } else if (up->oper == FIX) {
#if 0
            val = generate_rtl_call("fix", {L});
#else
            val = Builder.CreateFPToSI(L, Type::getInt32Ty(TheContext), "fix");
#endif
        } else {
            errs() << "Unary oper " << up->oper << " is not implemented\n";
        }
    } else if (auto np = dynamic_cast<TreeNumericalNode *>(expr)) {
        val = ConstantInt::get(Type::getInt32Ty(TheContext), np->num);
    } else if (auto np = dynamic_cast<TreeDNumericalNode *>(expr)) {
        val = ConstantFP::get(Type::getDoubleTy(TheContext), np->num);
    } else if (auto node = dynamic_cast<TreeIdentNode *>(expr)) {
        val = generate_load(node);
    } else if (auto node = dynamic_cast<TreeTextNode *>(expr)) {
        val = allocate_string_constant(node);
    } else if (auto np = dynamic_cast<TreeBooleanNode *>(expr)) {
        val = ConstantInt::get(Type::getInt1Ty(TheContext), np->num);
    } else {
        errs() << "Non-supported: " << typeid(*expr).name() << '\n';; 
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

// TODO: implement complex/struct types, arrays
Type *NodeToType(TreeNode *type)
{
    if(auto node = dynamic_cast<TreeUnaryNode *>(type)) {
        if(node->oper == T_STRING)
            return Type::getInt8PtrTy(TheContext);
        if(node->oper == T_REAL)
            return Type::getDoubleTy(TheContext);
        if(node->oper == T_BOOLEAN)
            return Type::getInt1Ty(TheContext);
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

Value *generate_rtl_call(const char *entry, std::vector<Value *> const &args)
{
    auto pos = rtl_symbols.find(entry);
    Value *val = 0;
    if(pos != rtl_symbols.end()) {
        if(Function *function = dynamic_cast<Function*>(pos->second)){
            val = Builder.CreateCall(function, args, "calltmp");
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
    else if (val[0]->getType() == Type::getInt1Ty(TheContext))
        generate_rtl_call("output_bool", val);
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
//     control: FOR(TO(1 BY(<null> 10)) while-cond)
//
//   MergeBB:
//           if NOT while-cond goto ElseBB;
//           if (index > to) goto ElseBB;
//  
//           <loop-body>
//   ThenBB:
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
        TreeNode *expr_step = 0;
        TreeNode *expr_to = 0;
        if(for_node->oper == FOR) {
            if(auto to_node = dynamic_cast<TreeBinaryNode *>(for_node->left)) {
                Value *init_expr = generate_expr(to_node->left);
                generate_store(loop_target, init_expr);

                if(auto by_node = dynamic_cast<TreeBinaryNode *>(to_node->right)) {
                    // by_node->oper == BY
                    expr_step = by_node->left; // ? generate_expr(by_node->left) : Builder.getInt32(1);
                    expr_to = by_node->right; // ? generate_expr(by_node->right) : 0;
                }
                
                auto if_stat = IfStatement(get_current_function());
                conditionals.push(if_stat);

                if(labels.size() && labels.top()->isForLoop()) {
                    labels.top()->RepentBB = if_stat.ElseBB; // loop exit
                    labels.top()->RepeatBB = if_stat.ThenBB; // loop continue
                }

                auto loop_stat = LoopStatement(loop_target, expr_step, expr_to);
                loops.push(loop_stat);

                Builder.CreateBr(if_stat.MergeBB);
                Builder.SetInsertPoint(if_stat.MergeBB);
                Value *index = generate_load(dynamic_cast<TreeIdentNode *>(loop_target));

                if(auto cond_control = for_node->right) {
                    // Generate "while(...)"
                    auto cont = BasicBlock::Create(TheContext, "to_label", get_current_function());
                    Value *cond_val = generate_expr(cond_control); // while(expr)
                    Value *Zero = Builder.getInt1(false);
                    Value *while_cond = Builder.CreateICmpNE(cond_val, Zero, "while_cond");
                    Builder.CreateCondBr(while_cond, cont, if_stat.ElseBB);
                    Builder.SetInsertPoint(cont);
                }

                if(expr_to) {
                    auto cont = BasicBlock::Create(TheContext, "loop_body", get_current_function());
                    Value *cmp = Builder.CreateICmpSLE(index, generate_expr(expr_to), "cmp");
                    Builder.CreateCondBr(cmp, cont, if_stat.ElseBB);
                    Builder.SetInsertPoint(cont);
                }
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
    // TODO: verify ident == label

    auto &cond = conditionals.top();
    auto &loop = loops.top();

    // ThenBB:
    //   index += step;

    Builder.CreateBr(cond.ThenBB);
    Builder.SetInsertPoint(cond.ThenBB);
    Value *index = generate_load(dynamic_cast<TreeIdentNode *>(loop.Target));

    //    index = Builder.CreateAdd(index, loop.By, "increment");
    Value *loop_by = loop.By ? generate_expr(loop.By) : Builder.getInt32(1);
    index = generate_add(index, loop_by, "increment");
    generate_store(loop.Target, index);
    Builder.CreateBr(cond.MergeBB);
    
    //Builder.CreateBr(cond.ElseBB);
    Builder.SetInsertPoint(cond.ElseBB);
    
    conditionals.pop();
    loops.pop();
}

// create a labelwhich preceed the for-loop
void set_for_label(TreeNode *node)
{
    auto ident = dynamic_cast<TreeIdentNode *>(node);
    assert(ident);

    auto label = new LabelStatement(ident->id);
    auto res = label_table.insert(std::make_pair(ident->id, label));
    // TODO: make sure the label is unique
    labels.push(label);
}

void set_label(TreeNode *node)
{
    auto ident = dynamic_cast<TreeIdentNode *>(node);
    assert(ident);

    auto label = new LabelStatement(get_current_function(), ident->id);
    auto res = label_table.insert(std::make_pair(ident->id, label));
    // TODO: make sure the label is unique
    labels.push(label);

    Builder.CreateBr(label->RepeatBB);
    Builder.SetInsertPoint(label->RepeatBB);
}

void clear_label()
{
    auto label = labels.top();
    labels.pop();

    auto res = label_table.erase(label->label);

    if(!label->isForLoop() && label->RepentBB) {
        Builder.CreateBr(label->RepentBB);
        Builder.SetInsertPoint(label->RepentBB);
    }
}

void make_repent(TreeNode *node)
{
    auto ident = dynamic_cast<TreeIdentNode *>(node);
    assert(ident);

    auto pos = label_table.find(ident->id);
    if(pos != label_table.end()) {
        LabelStatement *label = pos->second;
        Builder.CreateBr(label->getRepentBB());
    } else {
        // syntax error, label not found
        syntax_error(ident->id + ": label is unknown");
    }
}

void make_repeat(TreeNode *node)
{
    auto ident = dynamic_cast<TreeIdentNode *>(node);
    assert(ident);

    auto pos = label_table.find(ident->id);
    if(pos != label_table.end()) {
        LabelStatement *label = pos->second;
        Builder.CreateBr(label->RepeatBB);
    } else {
        // syntax error, label not found
        syntax_error(ident->id + ": label is unknown");
    }
}

void get_proc_arguments(TreeNode *lst, std::vector<Type *> &arg_types, std::vector<std::string> &arg_names)
{
    if(lst) {
        auto cp = dynamic_cast<TreeBinaryNode *>(lst);
        assert(cp);
        
        if(cp->oper == COMMA) {
            get_proc_arguments(cp->left, arg_types, arg_names);
            get_proc_arguments(cp->right, arg_types, arg_names);
        } else if (cp->oper == IDENT) {
            arg_names.push_back(dynamic_cast<TreeIdentNode *>(cp->left)->id);
            arg_types.push_back(NodeToType(cp->right));
        } else if (cp->oper == NAME) {
            // TODO: set flag "pass by name"
            arg_names.push_back(dynamic_cast<TreeIdentNode *>(cp->left)->id);
            arg_types.push_back(NodeToType(cp->right));
        } else {
            assert("Impossible!" == 0);
        }
    }
}       


std::stack<BasicBlock*> jumps; 

//
//    (FUNCTION,
//          (PROCEDURE,
//                <ident>,
//                (COMMA, <arg1>, <arg2>)
//          ),
//          <type>
//    )
//
void function_header(TreeNode *node)
{
    auto funct = dynamic_cast<TreeBinaryNode *>(node);
    assert(funct);

    if(funct->oper == T_FUNCTION) {
        auto proc = dynamic_cast<TreeBinaryNode *>(funct->left);
        Type *type = NodeToType(funct->right);

        assert(proc->oper == T_PROCEDURE);

        auto id = dynamic_cast<TreeIdentNode *>(proc->left);
        modules.push(new Module(id->id, TheContext));
        
        std::vector<Type *> arg_types;
        std::vector<std::string> arg_names;
        get_proc_arguments(proc->right, arg_types, arg_names);
        
        FunctionType *FT = FunctionType::get(type, arg_types, false);
        Function *F = Function::Create(FT, Function::PrivateLinkage, id->id, TheModule());

        // Set names for all arguments.
        int i = 0;
        for (auto &arg : F->args()) {
            arg.setName(arg_names[i]);
            auto res = symbols.insert(std::make_pair(arg_names[i], &arg)); 
            ++i;
        }

        // add function to the symbol table
        if(!insert_symbol(id->id, F))
            syntax_error(id->id + ": Cannot {re}define function name"); 
        
        BasicBlock *overBB = BasicBlock::Create(TheContext, "over_jump", get_current_function());
        Builder.CreateBr(overBB);
        jumps.push(overBB);
        
        BasicBlock *BB = BasicBlock::Create(TheContext, "entry", F);
        Builder.SetInsertPoint(BB);

        functions.push(F);
    }
}

std::string node_to_ident(TreeNode *node)
{
    auto ident = dynamic_cast<TreeIdentNode *>(node);
    assert(ident);
    return ident->id;
}

Value *get_default_value_of_type(Type *t)
{
    if(t->isDoubleTy())
        return ConstantFP::get(Type::getDoubleTy(TheContext), 0);
    if(t->isIntegerTy(1))
        return Builder.getInt1(false);
    return Builder.getInt32(0);
}

void function_end(TreeNode *node)
{
    auto F = get_current_function();
    verifyFunction(*F);

    functions.pop();
    // TODO: pop(); ... ; delete F;

#if 1
    // generate implicit return
    Value *rc = get_default_value_of_type(F->getReturnType());
    Builder.CreateRet(rc);
#endif

    // auto id = dynamic_cast<TreeIdentNode *>(node);
    // TODO: verify ending label == module name

    if(err_cnt == 0)
        TheModule()->print(outs(), nullptr);
    modules.pop();

    // restore previous function/programm
    BasicBlock *BB = jumps.top();
    jumps.pop();
    Builder.SetInsertPoint(BB);
}

void subroutine_end(TreeNode *node)
{
    errs() << "subroutine_end: " << node_to_ident(node) << "\n";
}

void return_statement()
{
    auto F = get_current_function();
    // generate return of "default" value of the function type
    Value *rc = get_default_value_of_type(F->getReturnType());
    Builder.CreateRet(rc);
}

void return_statement(TreeNode *node)
{
    Value *val = generate_expr(node);
    Builder.CreateRet(val);
}

bool insert_symbol(std::string const &s, Value *v)
{
    auto r = symbols.insert(std::make_pair(s, v));

    return r.second;
}


// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
