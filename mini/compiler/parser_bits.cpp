//
//
//

#include "parser.h"
#include "parser_bits.h"
#include "TreeNode.h"

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
#include "llvm/IR/Verifier.h"

#include <algorithm>
#include <cstdlib>
#include <stack>
#include <deque>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

#include "llvm_helper.h"
#include "symbol_type_table.h"

using namespace llvm;

typedef SmallVector<BasicBlock *, 16> BBList;
typedef SmallVector<Value *, 16> ValList;

LLVMContext TheContext;

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
        : f{}
        , RepeatBB{}
        , RepentBB{}
        , label{}
    {}

    LabelStatement(Function *f, std::string const &l)
        : f(f)
        , RepeatBB {createBB(f, "bb")}
        , RepentBB {}
        , label(l)
    {
    }

    // Label does not have branches. The branches will be set from the
    // labeled block (e.g. for-loop) later
    LabelStatement(std::string const &l) 
        : f {}
        , RepeatBB {}
        , RepentBB {}
        , label {l}
    {}

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


class FunctionContext {
public:
    std::unordered_map<std::string, Value *> symbols;
    Function *F = 0;

    FunctionContext(Function *f) : F{f} {}
};

struct dimension_t {
    Value *low;
    Value *up;

    dimension_t(Value *l, Value *u) : low(l), up(u) {} 
};

//  +------------+
//  | low bound  | 0
//  +------------+
//  | up bound   | 1
//  +------------+
//  | stride     | 2
//  +------------+
//  |  . . .     | ....
//  +------------+
//  | address    | n * dim_size
//  +------------+
//
enum array_t {
    low_bound = 0,
    up_bound = 1,
    stride = 2,
    dim_size = 3,
};

//
// prototypes
//

Value *initialize_array_type(Type *type, std::vector<dimension_t> const &dims, const char *symb);

static std::stack<LabelStatement *> labels;
static std::unordered_map<std::string, LabelStatement *> label_table;

// nested functions
std::unordered_map<std::string, Function *> fsymbols;

// run-time library
std::unordered_map<std::string, Function *> rtl_symbols;

// Map to track array element types for opaque pointer compatibility
std::unordered_map<StructType *, Type *> array_element_types;

//
// statics & globals
//

IRBuilder<> Builder(TheContext);
static std::stack<Module *> modules;
static Module  *TheModule()
{
    return modules.top();
}

static std::stack<FunctionContext> functions;

static std::stack<IfStatement> conditionals;
static std::stack<LoopStatement> loops;

int err_cnt = 0;
bool flag_verbose = false;

symbol_type_table type_table;

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
    insert_rtl_symbol("output_str", "rtl_output_str", Type::getInt32Ty(TheContext), {PointerType::getUnqual(Type::getInt8Ty(TheContext))});
    insert_rtl_symbol("output_real", "rtl_output_real", Type::getInt32Ty(TheContext), {Type::getDoubleTy(TheContext)});
    insert_rtl_symbol("output_bool", "rtl_output_bool", Type::getInt1Ty(TheContext), {Type::getInt1Ty(TheContext)});
    insert_rtl_symbol("output_nl", "rtl_output_nl", Type::getInt32Ty(TheContext), {});
    //    insert_rtl_symbol("fix", "rtl_fix", Type::getInt32Ty(TheContext),
    //    {Type::getDoubleTy(TheContext)});
    insert_rtl_symbol("allocate_array", "rtl_allocate_array",
                      PointerType::getUnqual(Type::getInt32Ty(TheContext)),
                      {Type::getInt32Ty(TheContext), Type::getInt32Ty(TheContext)});
}

//
// process main program
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
    for (auto &Arg : F->args())
        Arg.setName("arg");

    BasicBlock *BB = BasicBlock::Create(TheContext, "entry", F);
    Builder.SetInsertPoint(BB);

    set_current_function(F);
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

    functions_pop();
}

TreeNode *make_binary(TreeNode *left, TreeNode *right, int op)
{
    if (flag_verbose) {
        if (left && right)
            errs() << "[" << token_to_string(op) << "," << left->show() << "," << right->show()
                   << "]\n";
        else
            errs() << "[" << token_to_string(op) << "," << left->show() << ",<null>]\n";
    }
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

size_t get_field_offset(Type *type, TreeNode *node)
{
    size_t off = 0;
    if (flag_verbose)
        errs() << "get_field_offset: " << node->show() << "\n";
    assert(node->oper == IDENT);
    auto ident = dynamic_cast<TreeIdentNode *>(node);
    assert(ident);
    StructType *stype = cast<StructType>(type);
    assert(stype);
    auto fname = stype->getName() + "." + ident->id;
    Value *off_val = symbols_find(fname.str());
    if (off_val) {
        ConstantInt *cint = cast<ConstantInt>(off_val);
        assert(cint != 0);
        off = (size_t)cint->getLimitedValue();
    } else {
        syntax_error(ident->id + ": is not a name of a field");
    }
    return off;
}

//
// Generate appropriate Value for assignment target
// e.g.
//      a := ...
//      a[i][j] := ...
//      a.b := ...
//
//
Value *generate_lvalue(TreeNode *target)
{
    Value *lvalue = 0;
    if (auto node = dynamic_cast<TreeIdentNode *>(target)) {
        std::string id = node->id;
        auto pos = symbols_find(id);
        if (pos) {
            lvalue = pos;
        } else {
            syntax_error(id + ": ident not found");
        }
    } else if (target->oper == LBRACK) {
        std::stack<Value *> indexes;
        while (target->oper == LBRACK) {
            auto R = generate_expr(target->right);
            indexes.push(R);
            target = target->left;
        }
        if (auto ident = dynamic_cast<TreeIdentNode *>(target)) {
            auto sym = symbols_find(ident->id);
            if (!sym) {
                syntax_error(ident->id + ": not found");
                return lvalue; // null?
            }

            if (flag_verbose) {
                show_type_details(sym->getType()); // DEBUG
            }

            if (!isArrayType(sym)) {
                syntax_error(ident->id + ": is not array");
                return lvalue;
            }

            StructType *sym_type = array_get_type(sym);
            Type *array_elem_type = array_get_elem_type(sym_type);

            int off = 0;
            Value *I = Const(0);
            Value *R, *L;
            while (indexes.size()) {
                R = indexes.top();
                indexes.pop();

                auto LB = Builder.CreateGEP(sym_type, sym,
                                            {Const(0), Const(0), Const(off + array_t::low_bound)},
                                            "array_descr"); // low bound
                LB = Builder.CreateLoad(Type::getInt32Ty(TheContext), LB, "lb");
                R = Builder.CreateSub(R, LB, "sub_lb");
                auto S = Builder.CreateGEP(sym_type, sym,
                                           {Const(0), Const(0), Const(off + array_t::stride)},
                                           "stride_addr"); // stride for the current dimension
                S = Builder.CreateLoad(Type::getInt32Ty(TheContext), S, "stride");
                R = Builder.CreateMul(R, S, "R_mul_S");
                I = Builder.CreateAdd(I, R, "I_plus_R");
                off += array_t::dim_size;
            }
            L = Builder.CreateGEP(sym_type, sym, {Const(0), Const(1)}, "descr");
            Type *ptr_type = PointerType::getUnqual(array_elem_type);
            L = Builder.CreateLoad(ptr_type, L, "array_start");
            lvalue = Builder.CreateGEP(array_elem_type, L, {I}, "lvalue");
        } else {
            assert("Not implemented yet..." == 0);
            // generate_store(node->left, e);
            // generate_store(node->right, e);
        }
    } else if (target->oper == PERIOD) {
        if (auto ident = dynamic_cast<TreeIdentNode *>(target->left)) {
            auto sym = symbols_find(ident->id);
            if (!sym) {
                syntax_error(ident->id + ": not found");
                return lvalue; // null?
            }
            if (flag_verbose) {
                show_type_details(sym->getType());
                if (sym->getType())
                    sym->getType()->dump();
            }
            Type *struct_type = nullptr;
            if (auto *AI = dyn_cast<AllocaInst>(sym)) {
                struct_type = AI->getAllocatedType();
            } else {
                struct_type = sym->getType();
            }
            int off = get_field_offset(struct_type, target->right);
            lvalue = Builder.CreateStructGEP(struct_type, sym, off);
            if (flag_verbose) {
                errs() << "sym: " << sym << "\n";
                lvalue->dump();
            }
        } else {
            assert("Not implemented yet" == 0);
        }
    } else {
        assert("Not implemented yet" == 0);
    }
    return lvalue;
}

void generate_store(TreeNode *targets, Value *e)
{
    if (flag_verbose)
        errs() << "generate_store: " << targets->show() << "\n";

    if (targets->oper == BECOMES) {
        generate_store(targets->left, e);
        generate_store(targets->right, e);
    } else {
        auto lvalue = generate_lvalue(targets);
        Builder.CreateStore(e, lvalue);
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
    auto pos = symbols_find(id);
    if (pos) {
        if (is_loadable(pos, id)) {
            Type *load_type = nullptr;
            if (auto *AI = dyn_cast<AllocaInst>(pos)) {
                load_type = AI->getAllocatedType();
            } else {
                load_type = pos->getType();
            }
            val = Builder.CreateLoad(load_type, pos, "tmpvar");
        } else {
            val = pos;
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
    if (L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
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
    if (L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
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
    if (L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
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
    if (L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
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
    if (L->getType()->isFloatingPointTy() && R->getType()->isFloatingPointTy())
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
    if (L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
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
    if (L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
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
    if (L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
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
    if (L->getType()->isDoubleTy() && R->getType()->isDoubleTy())
        val = Builder.CreateFAdd(L, R, name);
    else
        val = Builder.CreateAdd(L, R, name);
    return val;
}

void build_actual_args(TreeNode *anode, std::vector<Value *> &args)
{
    if (auto bnode = dynamic_cast<TreeBinaryNode *>(anode)) {
        if (bnode->oper == COMMA) {
            build_actual_args(bnode->left, args);
            build_actual_args(bnode->right, args);
        } else {
            args.push_back(generate_expr(bnode));
        }
    } else {
        args.push_back(generate_expr(anode));
    }
}

Value *generate_call(TreeNode *fnode, TreeNode *anode)
{
    Value *val = 0;

    if (auto ident = dynamic_cast<TreeIdentNode *>(fnode)) {
        Value *F = symbols_find_function(ident->id);
        if (F) {
            std::vector<Value *> args;
            build_actual_args(anode, args);
            if (auto *Func = dyn_cast<Function>(F)) {
                val = Builder.CreateCall(Func->getFunctionType(), F, args, "fcall");
            } else {
                syntax_error(ident->id + ": Not a function");
            }
        } else {
            syntax_error(ident->id + ": Function name is not found");
        }
    } else {
        syntax_error("Function name must be ident");
    }

    return val;
}

bool isArrayType(Value *sym)
{
    return array_get_type(sym) != 0;
}

Value *resolve_array_symbol(TreeNode *node)
{
    Value *sym = 0;
    if (node->oper == IDENT) {
        auto ident = dynamic_cast<TreeIdentNode *>(node);
        assert(ident);
        sym = symbols_find(ident->id);
        if (!sym) {
            syntax_error(ident->id + ": not found");
            return 0;
        }
        if (!isArrayType(sym)) {
            syntax_error(ident->id + ": is not array");
            return 0;
        }
    } else if (node->oper == PERIOD) {
        sym = generate_dot(node);
    } else {
        // not implemented
        errs() << "resolve_array_symbol: " << node->show() << "\n";
        assert("Not implemented yet" == 0);
    }
    return sym;
}

Value *resolve_struct_symbol(TreeIdentNode *ident)
{
    Value *sym = symbols_find(ident->id);
    if (!sym) {
        syntax_error(ident->id + ": not found");
        return 0;
    }
    // TODO: implement isStructType
    // if(!isStructType(sym)) {
    //     syntax_error(ident->id + ": is not structure");
    //     return 0;
    // }
    return sym;
}

//
// E.g.   a.b
//        (PERIOD, a, b)
//
Value *generate_dot(TreeNode *dot)
{
    Value *val = 0;

    auto id = dynamic_cast<TreeIdentNode *>(dot->left);
    assert(id != 0);
    if (Value *sym = resolve_struct_symbol(id)) {
        Type *struct_type = nullptr;
        if (auto *AI = dyn_cast<AllocaInst>(sym)) {
            struct_type = AI->getAllocatedType();
        } else {
            struct_type = sym->getType();
        }
        int off = get_field_offset(struct_type, dot->right);
        auto LB = Builder.CreateStructGEP(struct_type, sym, off, "struct_fld");
        // val = Builder.CreateLoad(LB, "load_fld");
        val = LB;
    } else {
        syntax_error(id->id + ": cannot be resolved as a struct symbol");
    }
    return val;
}

Value *generate_dot_load(TreeNode *dot)
{
    Value *val = 0;

    auto id = dynamic_cast<TreeIdentNode *>(dot->left);
    assert(id != 0);
    if (Value *sym = resolve_struct_symbol(id)) {
        Type *struct_type = nullptr;
        if (auto *AI = dyn_cast<AllocaInst>(sym)) {
            struct_type = AI->getAllocatedType();
        } else {
            struct_type = sym->getType();
        }
        int off = get_field_offset(struct_type, dot->right);
        auto LB = Builder.CreateStructGEP(struct_type, sym, off, "struct_fld");

        // Get the type of the field we're loading
        StructType *stype = cast<StructType>(struct_type);
        Type *field_type = stype->getElementType(off);
        val = Builder.CreateLoad(field_type, LB, "load_fld");
    } else {
        syntax_error(id->id + ": cannot be resolved as a struct symbol");
    }

    return val;
}

//
//  NOTE: indexes are in reverse order
//
Value *generate_aij(Value *sym, std::vector<Value *> const &indexes)
{
    Value *val = 0;

    Value *zero = Builder.getInt32(0);
    StructType *arr_type = array_get_type(sym);
    Type *arr_elem_type = array_get_elem_type(arr_type);

    Value *I = Const(0);
    Value *R = Const(0);
    for (int i = 0; i != indexes.size(); ++i) {

        auto LB = Builder.CreateGEP(arr_type, sym,
                                    {zero, zero, Const(i * array_t::dim_size + array_t::low_bound)},
                                    "lb_addr"); // low bound
        LB = Builder.CreateLoad(Type::getInt32Ty(TheContext), LB, "lb");

        R = indexes[indexes.size() - i - 1]; // index
        R = Builder.CreateSub(R, LB, "r_lb");
        auto S = Builder.CreateGEP(arr_type, sym,
                                   {zero, zero, Const(i * array_t::dim_size + array_t::stride)},
                                   "stride_gep"); // stride
        S = Builder.CreateLoad(Type::getInt32Ty(TheContext), S, "stride");
        R = Builder.CreateMul(R, S, "r_mul_s");
        I = Builder.CreateAdd(I, R, "i_add_r");
    }

    auto L =
        Builder.CreateGEP(arr_type, sym, {zero, Const(1)}, "data_base_addr"); // data base address
    Type *ptr_type = PointerType::getUnqual(arr_elem_type);
    L = Builder.CreateLoad(ptr_type, L, "array_start");
    auto a_ij = Builder.CreateGEP(arr_elem_type, L, {I}, "a_ij");
    val = Builder.CreateLoad(arr_elem_type, a_ij, "load_a_ij");

    return val;
}

//
//  E.g.  a[i]  looks like:  ('[', (IDENT, a), (IDENT i))
//
Value *generate_aij(TreeNode *node1, TreeNode *node2)
{
    Value *val = 0;

    std::vector<Value *> indexes;
    indexes.push_back(generate_expr(node2));

    while (node1->oper == LBRACK) {
        node2 = node1->right;
        indexes.push_back(generate_expr(node2));
        node1 = node1->left;
    }
    val = generate_aij(resolve_array_symbol(node1), indexes);

    return val;
}

Value *generate_expr(TreeNode *expr)
{
    Value *val = 0;

    if (flag_verbose)
        errs() << "generate_expr: " << typeid(*expr).name() << '\n';

    if (auto bp = dynamic_cast<TreeBinaryNode *>(expr)) {
        if (bp->oper == CALLSYM)
            val = generate_call(bp->left, bp->right);
        else if (bp->oper == LBRACK)
            val = generate_aij(bp->left, bp->right);
        else if (bp->oper == PERIOD)
            val = generate_dot_load(bp);
        else {
            Value *L = generate_expr(expr->left);
            Value *R = generate_expr(expr->right);
            if (bp->oper == PLUS)
                val = generate_add(L, R);
            else if (bp->oper == MINUS)
                val = generate_sub(L, R);
            else if (bp->oper == TIMES)
                val = generate_mul(L, R);
            else if (bp->oper == SLASH)
                val = generate_div(L, R);
            else if (bp->oper == GTR)
                val = generate_compare_gtr_expr(L, R);
            else if (bp->oper == LEQ)
                val = generate_compare_leq_expr(L, R);
            else if (bp->oper == LSS)
                val = generate_compare_lss_expr(L, R);
            else if (bp->oper == GEQ)
                val = generate_compare_geq_expr(L, R);
            else if (bp->oper == EQL)
                val = generate_compare_eql_expr(L, R);
            else if (bp->oper == AND)
                val = Builder.CreateAnd(L, R, "andtmp");
            else if (bp->oper == OR)
                val = Builder.CreateOr(L, R, "ortmp");
            else
                errs() << "Not implemented op: " << token_to_string(bp->oper) << "\n";
        }
    } else if (auto up = dynamic_cast<TreeUnaryNode *>(expr)) {
        Value *L = generate_expr(expr->left);
        if (up->oper == MINUS) {
            if (L->getType()->isFloatingPointTy())
                val = Builder.CreateFNeg(L, "fneg");
            else
                val = Builder.CreateNeg(L, "neg");
        } else if (up->oper == FIX) {
#if 0
            val = generate_rtl_call("fix", {L});
#else
            val = Builder.CreateFPToSI(L, Type::getInt32Ty(TheContext), "fix");
#endif
        } else if (up->oper == FLOAT) {
            val = Builder.CreateSIToFP(L, Type::getDoubleTy(TheContext), "float");
        } else {
            errs() << "Unary oper " << token_to_string(up->oper) << " is not implemented\n";
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
        errs() << "Non-supported: " << typeid(*expr).name() << '\n';
        ;
    }

    if (val == 0)
        errs() << "val == 0\n";
    return val;
}

void assign_statement(TreeNode *targets, TreeNode *expr)
{
    if (flag_verbose)
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
    if (vars == 0)
        return;
    if (auto bn = dynamic_cast<TreeBinaryNode *>(vars)) {
        get_ids(bn->left, res);
        get_ids(bn->right, res);
    } else if (auto id = dynamic_cast<TreeIdentNode *>(vars)) {
        res.push_back(id->id);
    }
}

type_value_t create_alloca(Type *t, const char *s)
{
    Value *v = s ? Builder.CreateAlloca(t, 0, s) : 0;
    return type_value_t(t, v);
}

///
///
///
std::string compose_tmp_struct_name()
{
    static size_t serial = 0;

    return std::string("struct_") + get_current_function()->getName().str() + "_" +
           std::to_string(++serial);
}

void build_field_list(TreeNode *anode, std::vector<TreeNode *> &fields)
{
    if (anode->oper == COMMA) {
        build_field_list(anode->left, fields);
        build_field_list(anode->right, fields);
    } else {
        fields.push_back(anode);
    }
}

std::string field_name(TreeNode *node)
{
    if (flag_verbose)
        errs() << "field_name: " << node->show() << "\n";
    assert(node->oper == FIELD);
    if (auto id = dynamic_cast<TreeIdentNode *>(node->left))
        return id->id;
    return "<none>";
}

///
///
///
symbol_type *construct_structure_type(TreeNode *node, std::string const &sname)
{
    symbol_type *stype = 0;

    std::vector<TreeNode *> fields;
    build_field_list(node, fields);

    std::vector<Type *> ftypes;
    size_t off = 0;
    for (TreeNode *fld : fields) {
        auto fname = sname + "." + field_name(fld);
        if (flag_verbose)
            errs() << "FIELD: " << fname << "\n";
        Type *ftype = node_to_type(fld->right);
        ftypes.push_back(ftype);
        auto off_value = Builder.getInt32(off++);
        symbols_insert(fname, off_value);
        if (flag_verbose)
            off_value->dump();
    }

    stype = new symbol_type(sname, 0, CreateStructType(ftypes, sname));
    return stype;
}

type_value_t node_to_type(TreeNode *node, const char *sym)
{
    if (node->oper == T_STRING)
        return create_alloca(PointerType::getUnqual(Type::getInt8Ty(TheContext)), sym);
    if (node->oper == T_REAL)
        return create_alloca(Type::getDoubleTy(TheContext), sym);
    if (node->oper == T_BOOLEAN)
        return create_alloca(Type::getInt1Ty(TheContext), sym);
    if (node->oper == ARRAY) {
        // array of arrays will be converted into multi-dimensional arrays
        std::vector<dimension_t> dims;
        Value *val = 0;
        Type *type = 0;
        do {
            auto bounds = node->left;
            auto L = generate_expr(bounds->left);
            auto R = bounds->right ? generate_expr(bounds->right) : Builder.getInt32(1);
            dims.push_back(dimension_t(R, L));
            node = node->right;
        } while (node->oper == ARRAY);

        Type *item_type = node_to_type(node);
        type = CreateArrayType(item_type, dims.size());
        if (sym)
            val = initialize_array_type(type, dims, sym);
        return type_value_t(type, val);
    }
    if (node->oper == STRUCTURE) {
        Type *type = 0;

        std::string type_name = compose_tmp_struct_name();
        auto t = construct_structure_type(node->left, type_name);
        assert(t);
        type = t->type;

        return create_alloca(type, sym);
    }

    return create_alloca(Type::getInt32Ty(TheContext), sym);
}

Type *getArrayElementPointerTy(Type *array)
{
    StructType *s_type = cast<StructType>(array);
    return s_type->elements().back();
}

//
// type is struct type to reprezent the passport of an array
//
size_t getSizeofArrayElement(Type *type)
{
    Type *pointer_type = getArrayElementPointerTy(type);
    if (pointer_type == PointerType::getUnqual(Type::getInt32Ty(TheContext)))
        return 4;
    return 8;
}

Value *Const(int c)
{
    return Builder.getInt32(c);
}

Value *initialize_array_type(Type *type, std::vector<dimension_t> const &dims, const char *sym)
{
    Value *val = Builder.CreateAlloca(type, 0, sym);
    StructType *struct_type = cast<StructType>(type);

    Value *total = Builder.getInt32(1);
    std::stack<Value *> strides;

    for (int i = 0; i != dims.size(); ++i) {
        auto Low = dims[i].low;
        auto Up = dims[i].up;

        auto pos = Builder.CreateGEP(
            struct_type, val,
            {Const(0), Const(0), Const(i * array_t::dim_size + array_t::low_bound)});
        Builder.CreateStore(Low, pos);

        pos = Builder.CreateGEP(
            struct_type, val,
            {Const(0), Const(0), Const(i * array_t::dim_size + array_t::up_bound)});
        Up = Builder.CreateAdd(Up, Const(1));
        Builder.CreateStore(Up, pos);

        auto len = Builder.CreateSub(Up, Low);
        strides.push(len);
        total = Builder.CreateMul(total, len);
    }

    Value *stride = Const(1);

    for (int i = dims.size(); i--;) {
        int off = i * array_t::dim_size;
        auto pos =
            Builder.CreateGEP(struct_type, val, {Const(0), Const(0), Const(off + array_t::stride)});
        Builder.CreateStore(stride, pos);
        if (i) {
            stride = Builder.CreateMul(stride, strides.top());
            strides.pop();
        }
    }

    size_t sz = getSizeofArrayElement(type);

    auto array_mem = generate_rtl_call("allocate_array", {total, Builder.getInt32(sz)});
    array_mem = Builder.CreatePointerCast(array_mem, getArrayElementPointerTy(type));
    auto pos = Builder.CreateStructGEP(struct_type, val, 1);
    Builder.CreateStore(array_mem, pos);
    return val;
}

Type *node_to_type(TreeNode *type)
{
    return node_to_type(type, 0).first;
}

void variable_declaration(TreeNode *variables, TreeNode *type)
{
    if (flag_verbose)
        errs() << "variable_declaration: type=" << type->show() << "\n";

    std::vector<std::string> names;
    get_ids(variables, names);
    for (auto s : names) {
        // allocate memory for the variable of the type
        Value *symb = generate_alloca(type, s);
        auto res = symbols_insert(s, symb);
        assert(res);
    }
}

Value *generate_alloca(TreeNode *type, std::string const &s)
{
    return node_to_type(type, s.c_str()).second;
}

Value *generate_rtl_call(const char *entry, std::vector<Value *> const &args)
{
    auto pos = rtl_symbols.find(entry);
    Value *val = 0;
    if (pos != rtl_symbols.end()) {
        if (Function *function = dynamic_cast<Function *>(pos->second)) {
            val = Builder.CreateCall(function, args, "calltmp");
        }
    } else {
        ++err_cnt;
        errs() << "generate_rtl_call: " << entry << " not defined\n";
    }
    return val;
}

void generate_output_call(std::vector<Value *> const &val)
{
    if (val[0]->getType() == PointerType::getUnqual(Type::getInt8Ty(TheContext)))
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

    if (auto node = dynamic_cast<TreeBinaryNode *>(expr)) {
        if (node->oper == COMMA) {
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

    if (append_nl) {
        std::vector<Value *> val(0); // no args
        generate_rtl_call("output_nl", val);
    }

    return 0;
}

Function *get_current_function()
{
    return functions.size() ? functions.top().F : 0;
}

void cond_specification(TreeNode *expr)
{
    auto if_stat = IfStatement(get_current_function());

    conditionals.push(if_stat);
    Value *Compare = generate_expr(expr);

    Value *Condtn {};
    if (Compare->getType() == Type::getInt1Ty(TheContext)) {
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
    if (Condtn) {
        Builder.CreateCondBr(Condtn, if_stat.ThenBB, if_stat.ElseBB);
        Builder.SetInsertPoint(if_stat.ThenBB);
    }
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
    if (flag_verbose) {
        errs() << "loop_target: " << loop_target->show() << "\n";
        errs() << "control: " << control->show() << "\n";
    }

    if (auto for_node = dynamic_cast<TreeBinaryNode *>(control)) {
        TreeNode *expr_step = 0;
        TreeNode *expr_to = 0;
        if (for_node->oper == FOR) {
            if (auto to_node = dynamic_cast<TreeBinaryNode *>(for_node->left)) {
                Value *init_expr = generate_expr(to_node->left);
                generate_store(loop_target, init_expr);

                if (auto by_node = dynamic_cast<TreeBinaryNode *>(to_node->right)) {
                    // by_node->oper == BY
                    expr_step =
                        by_node->left; // ? generate_expr(by_node->left) : Builder.getInt32(1);
                    expr_to = by_node->right; // ? generate_expr(by_node->right) : 0;
                }

                auto if_stat = IfStatement(get_current_function());
                conditionals.push(if_stat);

                if (labels.size() && labels.top()->isForLoop()) {
                    labels.top()->RepentBB = if_stat.ElseBB; // loop exit
                    labels.top()->RepeatBB = if_stat.ThenBB; // loop continue
                }

                auto loop_stat = LoopStatement(loop_target, expr_step, expr_to);
                loops.push(loop_stat);

                Builder.CreateBr(if_stat.MergeBB);
                Builder.SetInsertPoint(if_stat.MergeBB);
                Value *index = generate_load(dynamic_cast<TreeIdentNode *>(loop_target));

                if (auto cond_control = for_node->right) {
                    // Generate "while(...)"
                    auto cont = BasicBlock::Create(TheContext, "to_label", get_current_function());
                    Value *cond_val = generate_expr(cond_control); // while(expr)
                    Value *Zero = Builder.getInt1(false);
                    Value *while_cond = Builder.CreateICmpNE(cond_val, Zero, "while_cond");
                    Builder.CreateCondBr(while_cond, cont, if_stat.ElseBB);
                    Builder.SetInsertPoint(cont);
                }

                if (expr_to) {
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

    // Builder.CreateBr(cond.ElseBB);
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
    (void)res; // silence warnings
    if (!label->isForLoop() && label->RepentBB) {
        Builder.CreateBr(label->RepentBB);
        Builder.SetInsertPoint(label->RepentBB);
    }
}

void make_repent(TreeNode *node)
{
    auto ident = dynamic_cast<TreeIdentNode *>(node);
    assert(ident);

    auto pos = label_table.find(ident->id);
    if (pos != label_table.end()) {
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
    if (pos != label_table.end()) {
        LabelStatement *label = pos->second;
        Builder.CreateBr(label->RepeatBB);
    } else {
        // syntax error, label not found
        syntax_error(ident->id + ": label is unknown");
    }
}

void get_proc_arguments(TreeNode *lst, std::vector<Type *> &arg_types,
                        std::vector<std::string> &arg_names)
{
    if (lst) {
        auto cp = dynamic_cast<TreeBinaryNode *>(lst);
        assert(cp);

        if (cp->oper == COMMA) {
            get_proc_arguments(cp->left, arg_types, arg_names);
            get_proc_arguments(cp->right, arg_types, arg_names);
        } else if (cp->oper == IDENT) {
            arg_names.push_back(dynamic_cast<TreeIdentNode *>(cp->left)->id);
            arg_types.push_back(node_to_type(cp->right));
        } else if (cp->oper == NAME) {
            // TODO: set flag "pass by name"
            arg_names.push_back(dynamic_cast<TreeIdentNode *>(cp->left)->id);
            arg_types.push_back(node_to_type(cp->right));
        } else {
            assert("Impossible!" == 0);
        }
    }
}

std::stack<BasicBlock *> jumps;

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

    if (funct->oper == T_FUNCTION) {
        auto proc = dynamic_cast<TreeBinaryNode *>(funct->left);
        Type *type = node_to_type(funct->right);

        assert(proc->oper == T_PROCEDURE);

        auto id = dynamic_cast<TreeIdentNode *>(proc->left);
        //modules.push(new Module(id->id, TheContext));

        std::vector<Type *> arg_types;
        std::vector<std::string> arg_names;
        get_proc_arguments(proc->right, arg_types, arg_names);

        FunctionType *FT = FunctionType::get(type, arg_types, false);
        Function *F = Function::Create(FT, Function::PrivateLinkage, id->id, TheModule());

        // add function to the symbol table (the previous one)
        if (!symbols_insert_function(id->id, F))
            syntax_error(id->id + ": Cannot {re}define function name");

        BasicBlock *overBB = BasicBlock::Create(TheContext, "over_jump", get_current_function());
        Builder.CreateBr(overBB);
        jumps.push(overBB);

        // create new symbol table
        set_current_function(F);
        // Set names for all arguments.
        int i = 0;
        for (auto &arg : F->args()) {
            arg.setName(arg_names[i]);
            auto res = symbols_insert(arg_names[i], &arg);
            assert(res);
            ++i;
        }

        BasicBlock *BB = BasicBlock::Create(TheContext, "entry", F);
        Builder.SetInsertPoint(BB);
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
    if (t->isDoubleTy())
        return ConstantFP::get(Type::getDoubleTy(TheContext), 0);
    if (t->isIntegerTy(1))
        return Builder.getInt1(false);
    return Builder.getInt32(0);
}

/// @brief End of internal function definition.
/// @param node 
void function_end(TreeNode *node)
{
    auto F = get_current_function();
    verifyFunction(*F);

    F->dump(); // DEBUG
    functions_pop();
    // TODO: pop(); ... ; delete F;

#if 1
    // generate implicit return
    Value *rc = get_default_value_of_type(F->getReturnType());
    Builder.CreateRet(rc);
#endif

    // auto id = dynamic_cast<TreeIdentNode *>(node);
    // TODO: verify ending label == module name

    if (err_cnt == 0)
        TheModule()->print(outs(), nullptr);
    //modules.pop();

    // restore previous function/program
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

void symbols_dump()
{
    if (functions.size()) {
        errs() << "F: " << (functions.top().F ? functions.top().F->getName() : "<none>") << "\n";
        for (auto item : functions.top().symbols) {
            errs() << "\t" << item.first << "\n";
        }
    } else {
        errs() << "Empty function stack";
    }
}

bool symbols_insert(std::string const &s, Value *v)
{
    auto r = functions.top().symbols.insert(std::make_pair(s, v));

    return r.second;
}

bool symbols_insert_function(std::string const &s, Function *v)
{
    auto r = fsymbols.insert(std::make_pair(s, v));

    return r.second;
}

Value *symbols_find(std::string const &id)
{
    auto pos = functions.top().symbols.find(id);
    return pos == functions.top().symbols.end() ? 0 : pos->second;
}

Value *symbols_find_function(std::string const &id)
{
    auto pos = fsymbols.find(id);
    return pos == fsymbols.end() ? 0 : pos->second;
}

//
//
//
TreeNode *type_identifier(TreeNode *node)
{
    errs() << "type_identifier: " << typeid(*node).name() << '\n';
    return node;
}

Type *CreateArrayType(Type *item_type, size_t ndims)
{
    std::vector<Type *> types;

    Type *vecTy = ArrayType::get(Type::getInt32Ty(TheContext), array_t::dim_size * ndims);
    types.push_back(vecTy);
    Type *elem_type = item_type ? item_type : Type::getInt32Ty(TheContext);
    Type *ptr = PointerType::getUnqual(elem_type);
    types.push_back(ptr);

    StructType *result = StructType::get(TheContext, TypeArray(types));
    // Store the element type for later retrieval
    array_element_types[result] = elem_type;
    return result;
}

Type *CreateStructType(std::vector<Type *> items, std::string const &name)
{
    StructType *t = StructType::get(TheContext, TypeArray(items));
    if (!t->isLiteral())
        t->setName(name);
    return t;
}

Type *CreateStructType(Type *item, size_t n)
{
    std::vector<Type *> types;

    Type *vecTy = ArrayType::get(item, n);
    types.push_back(vecTy);

    return StructType::get(TheContext, TypeArray(types));
}

//
// sym is an "expression" from symbol table (allocation result)
//
StructType *array_get_type(Value *sym)
{
    if (auto *AI = dyn_cast<AllocaInst>(sym)) {
        if (AI->getAllocatedType()->isStructTy())
            return cast<StructType>(AI->getAllocatedType());
    }
    return 0;
}

Type *array_get_elem_type(StructType *arr_type)
{
    auto it = array_element_types.find(arr_type);
    if (it != array_element_types.end()) {
        return it->second;
    }
    // Fallback to int32 if not found
    return Type::getInt32Ty(TheContext);
}

//
//
//
void type_declaration(TreeNode *ident_node, TreeNode *type_node)
{
    TreeIdentNode *ident = dynamic_cast<TreeIdentNode *>(ident_node);
    assert(ident);
    Type *type = node_to_type(type_node);

#if 0
    // TODO: save definition in type table
    symbols_insert(ident->id, type);
#else
    (void)type;
#endif
}

llvm::LLVMContext *get_global_context()
{
    return &TheContext;
}

void set_current_function(Function *F)
{
    functions.push(F);
}

void functions_pop()
{
    if (flag_verbose)
        symbols_dump();
    functions.pop(); // TODO: delete ?
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
