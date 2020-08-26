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
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

using namespace llvm;

//
// prototypes
//
Value *generate_load(TreeIdentNode *node);

std::unordered_map<std::string, llvm::Value *> symbols;
std::unordered_map<std::string, llvm::Function *> rtl_symbols;

static llvm::LLVMContext TheContext;
static llvm::IRBuilder<> Builder(TheContext);
static std::unique_ptr<llvm::Module> TheModule;

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

static llvm::Function *F;
void program_header(TreeNode *node)
{
    auto id = dynamic_cast<TreeIdentNode *>(node);

    TheModule = llvm::make_unique<llvm::Module>(id->id, TheContext);

    init_rtl_symbols();
    
    std::vector<llvm::Type *> Doubles(0, llvm::Type::getDoubleTy(TheContext));
    llvm::FunctionType *FT =
        llvm::FunctionType::get(Builder.getInt32Ty(), Doubles, false);

    F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "main", TheModule.get());

    // Set names for all arguments.
    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName("arg");

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);
}

void program_end(TreeNode *node)
{
    auto rc = Builder.getInt32(0);

    Builder.CreateRet(rc);

    verifyFunction(*F);

    auto id = dynamic_cast<TreeIdentNode *>(node);
    // TODO: verify ending label == module name

    if(err_cnt == 0)
        TheModule->print(llvm::outs(), nullptr);
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

void generate_store(TreeNode *targets, llvm::Value *e)
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

llvm::Value *generate_expr(TreeNode *expr)
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

void generate_rtl_call(const char *entry, std::vector<Value *> &args)
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

TreeNode *make_output(TreeNode *expr, bool append_nl)
{
    //  make_output: 14TreeBinaryNode
    //  make_output: 17TreeNumericalNode
    //  make_output: 13TreeIdentNode

    if(auto node = dynamic_cast<TreeBinaryNode *>(expr)) {
        if(node->oper == COMMA) {
            std::vector<Value *> val {generate_expr(node->left)};
            generate_rtl_call("output", val);
            make_output(node->right);
        } else {
            std::vector<Value *> val {generate_expr(node)};
            generate_rtl_call("output", val); 
        }
    } else {
        std::vector<Value *> val {generate_expr(expr)};
        if(val[0]->getType() == Type::getInt8PtrTy(TheContext))
            generate_rtl_call("output_str", val);
        else if (val[0]->getType() == Type::getDoubleTy(TheContext))
            generate_rtl_call("output_real", val);
        else
            generate_rtl_call("output", val);
    }

    if(append_nl) {
        std::vector<Value *> val(0); //no args
        generate_rtl_call("output_nl", val);
    }
    
    return 0;
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
