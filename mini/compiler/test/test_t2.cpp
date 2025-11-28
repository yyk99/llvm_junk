//
//
//

#include "CompilerTestsBase.h"

class T2 : public CompilerTestBase {
protected:
    virtual void SetUp() override
    {
        TheModule = new Module(current_case_name(), C);

        std::vector<Type *> formal_args(0);
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(C), formal_args, false);
        F = Function::Create(FT, Function::ExternalLinkage, current_test_name(), TheModule);

        set_current_function(F);

        BasicBlock *BB = llvm::BasicBlock::Create(C, "entry", F);
        Builder.SetInsertPoint(BB);
    }

    virtual void TearDown() override
    {
        Builder.CreateRet(Builder.getInt32(0));
        verifyFunction(*F);

        if (verbose)
            TheModule->print(errs(), nullptr);

        functions_pop();

        F->eraseFromParent();

        delete TheModule;
        TheModule = 0;
    }
};

TEST_F(T2, current_test_name)
{
    EXPECT_EQ("current_test_name", current_test_name());
}

TEST_F(T2, current_case_name)
{
    EXPECT_EQ("T2", current_case_name());
}

TEST_F(T2, CreateArrayType)
{
    auto type = CreateArrayType(Type::getInt32Ty(C), 1);
    ASSERT_TRUE(type);

    show_type_details(type);
    type->dump();
}

TEST_F(T2, get_current_function)
{
    Function *actual = get_current_function();
    ASSERT_TRUE(actual);
}

TEST_F(T2, CreateArrayType2)
{
    std::vector<Type *> types;

    int ndims = 2;

    Type *vecTy = ArrayType::get(Type::getInt32Ty(C), 3 * ndims);
    types.push_back(vecTy);
    Type *ptr = PointerType::getUnqual(Type::getInt32Ty(C));
    types.push_back(ptr);

    Type *type = StructType::get(C, TypeArray(types));

    Value *array = Builder.CreateAlloca(type, 0, "array");
    ASSERT_TRUE(isDynamicArrayType(array));

    // emit initialization for the array variable
    auto Low = Builder.getInt32(1);
    auto Up = Builder.getInt32(10);
    auto Zero = Builder.getInt32(0);
    auto One = Builder.getInt32(1);

    Value *header = Builder.CreateStructGEP(type, array, 0);
    ASSERT_TRUE(header);

    show_type_details(header->getType());

    auto *arr_type = cast<ArrayType>(cast<StructType>(type)->getElementType(0));
    Value *pos0 = Builder.CreateGEP(arr_type, header, {Zero, Zero});
    Builder.CreateStore(Low, pos0);

    Value *pos1 = Builder.CreateGEP(arr_type, header, {Zero, One});
    Builder.CreateStore(Up, pos1);
}

TEST_F(T2, CreateArrayType3)
{
    std::vector<Type *> types;

    int ndims = 2;

    Type *vecTy = ArrayType::get(Type::getInt32Ty(C), 3 * ndims);
    types.push_back(vecTy);
    Type *ptr = PointerType::getUnqual(Type::getInt32Ty(C));
    types.push_back(ptr);

    Type *type = StructType::get(C, TypeArray(types));

    Value *header = Builder.CreateAlloca(type, 0, "array");
    ASSERT_TRUE(isDynamicArrayType(header));

    // emit initialization for the array variable
    auto Low = Builder.getInt32(1);
    auto Up = Builder.getInt32(10);
    auto Zero = Builder.getInt32(0);
    auto One = Builder.getInt32(1);

    auto *struct_type = cast<StructType>(type);
    Value *pos0 = Builder.CreateGEP(struct_type, header, {Zero, Zero, Zero});
    Builder.CreateStore(Low, pos0);

    Value *pos1 = Builder.CreateGEP(struct_type, header, {Zero, Zero, One});
    Builder.CreateStore(Up, pos1);
}

TEST_F(T2, isArrayType)
{
    Type *array = CreateArrayType(Type::getInt32Ty(C), 1);
    ASSERT_NE(nullptr, array);

    Value *val = Builder.CreateAlloca(array, 0, "array");
    ASSERT_TRUE(isDynamicArrayType(val));
}

TEST_F(T2, node_to_type_structure)
{
    ASSERT_TRUE(Builder.GetInsertBlock());
    // 0. create
    Value *dummy = Builder.CreateAlloca(Type::getInt32Ty(C), 0, "dummy");
    ASSERT_TRUE(dummy);

    // 1. construct TreeNode
    // [STRUCTURE,COMMA(FIELD(first T_REAL(<null>)) FIELD(second T_REAL(<null>))),<null>]

    auto *_1 = make_binary(new TreeIdentNode("first"), base_type(T_REAL), FIELD);
    auto *_3 = make_binary(new TreeIdentNode("second"), base_type(T_REAL), FIELD);
    auto *_2 = make_binary(_1, _3, COMMA);
    auto *s_node = make_binary(_2, 0, STRUCTURE);

    std::cout << "s_node: " << s_node->show() << "\n";

    // 2. call it
    // type_value_t NodeToType(TreeNode *node, const char *sym)
    // typedef std::pair<llvm::Type *, llvm::Value *> type_value_t;
    ASSERT_TRUE(Builder.GetInsertBlock() != 0);
    type_value_t actual = node_to_type(s_node, "foo");

    // 3. Verify
    ASSERT_TRUE(actual.first);
    Type *type = actual.first;
    ASSERT_TRUE(actual.second);
    Value *val = actual.second;
    val->dump();

    show_type_details(type);
    type->dump();
}

TEST_F(T2, create_nested_function)
{
    verbose = true;
#if 0
    auto ws = create_workspace();
    auto output_file = ws / "output.ll";

    // Write LLVM IR to file
    std::error_code EC;
    llvm::raw_fd_ostream output(output_file.string(), EC);
    ASSERT_FALSE(EC) << "Failed to open output file: " << EC.message();

    TheModule->print(output, nullptr);
    output.close();

    // Verify file was created and has content
    ASSERT_TRUE(fs::is_regular_file(output_file)) << "Output file was not created";
    EXPECT_LE(0, fs::file_size(output_file)) << "Output file is empty";
#endif

#if 0
    std::vector<Type *> formal_args(0);
    formal_args.push_back(Type::getInt32Ty(C));

    FunctionType *FT = FunctionType::get(Type::getInt32Ty(C), formal_args, false);
    auto F = Function::Create(FT, Function::ExternalLinkage, current_test_name() + "_a", TheModule);
    BasicBlock *BB = llvm::BasicBlock::Create(C, "entry_a", F);
    Builder.SetInsertPoint(BB);
    Builder.CreateRet(Builder.getInt32(42));
    verifyFunction(*F);
#endif
}

#include "parser.h"
#include "parser_bits.h"

extern int yyparse();
extern int yylineno;

class CompilerF : public CompilerTestBase {
protected:
    void SetUp() override
    {
        enable_flag_verbose(false);
    }

    bool save_as_text(std::string const &text, fs::path const &as)
    {
        std::ofstream ss(as);
        if (!ss.good())
            return false;
        ss << text;
        return ss.good();
    }

    void enable_yydebug(int v)
    {
#ifdef YYDEBUG
        extern int yydebug;
        yydebug = v;
#endif
    }

    void enable_flag_verbose(bool v)
    {
        flag_verbose = v;
    }
};

TEST_F(CompilerF, function_abs)
{
    auto ws = create_workspace();

    char const *sample = R"(/* function example */
program FUNC:
    declare (f, b, a0) integer;
    declare a real;

    function abs (x real) real :
        if x < 0 then return -x; else return x; fi;
    end function abs;

    output abs(-abs(10.0) * (-2));
    output abs(-10.0);

    return;
end program FUNC;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));

    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler();
    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

TEST_F(CompilerF, hello_world)
{
    auto ws = create_workspace();

    char const *sample = R"(/* hello world example  */
program hello_world:
    declare hello string;
    set hello := "Hello world";
    output hello;
end program hello_world;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler();
    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

/// @brief Test for struct data type
/// @param --gtest_filter=CompilerF.struct_test
TEST_F(CompilerF, struct_test)
{
    auto ws = create_workspace();

    char const *sample = R"(/* struct_test */
program StructSample:
    declare p structure field first is real, field second is integer end structure;
    set p.first := 1.5;
    set p.second := 3;
    output p.first, p.second;
end program StructSample;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler(sample_mini.string().c_str());
    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

/// @brief Test for struct types with array field
/// @param --gtest_filter=CompilerF.struct_array_test
TEST_F(CompilerF, struct_array_test)
{
    auto ws = create_workspace();

    char const *sample = R"(/* struct_array_test  */
program StructSample:
    declare p structure field third is array [10] of integer
    end structure;
    output p.third[1];
end program StructSample;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler();
    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

/// @brief Test for struct types with array field as lvalue
/// @param --gtest_filter=CompilerF.struct_array_lvalue_test
TEST_F(CompilerF, struct_array_lvalue_test)
{
    auto ws = create_workspace();

    char const *sample = R"(/* struct_array_lvalue_test  */
program StructSample:
    declare p structure field third is array [10] of integer end structure;
    set p.third[1] := 567;
    output p.third[1];
end program StructSample;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler(sample_mini.string().c_str());

    enable_flag_verbose(true);

    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

/// @brief Test for struct types with struct field as lvalue
/// @param --gtest_filter=CompilerF.struct_struct_lvalue_test
TEST_F(CompilerF, struct_struct_lvalue_test)
{
    auto ws = create_workspace();

    char const *sample = R"(/* struct_struct_lvalue_test  */
program StructSample:
    declare p
        structure
           field a is array [10] of integer,
           field s is structure
                        field x is int,
                        field y is int
                      end structure
        end structure;
    set p.s.x := 99;
    set p.s.y := 88;
    set p.a[1] := 567;
    output p.a[1];
end program StructSample;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler(sample_mini.string().c_str());

    enable_flag_verbose(true);

    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

/// @brief Test for struct types with struct field as value
/// @param --gtest_filter=CompilerF.struct_struct_value_test
TEST_F(CompilerF, struct_struct_value_test)
{
    auto ws = create_workspace();

    char const *sample = R"(/* struct_struct_value_test  */
program StructSample:
    declare p
        structure
           field a is array [10] of integer,
           field s is structure
                        field x is int,
                        field y is int
                      end structure
        end structure;
    set p.s.x := 99;
    set p.s.y := 88;
    set p.a[1] := 567;
    output p.a[1];
    output p.s.x;
end program StructSample;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler(sample_mini.string().c_str());

    enable_flag_verbose(true);

    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

/// @brief Test for nested struct types
/// @param --gtest_filter=CompilerF.struct_struct_test
TEST_F(CompilerF, struct_struct_test)
{
    auto ws = create_workspace();

    char const *sample = R"(/* struct_struct_test  */
program StructSample:
    declare p
        structure
           field a is integer,
           field s is structure
                        field x is int,
                        field y is int
                      end structure
        end structure;
    output p.s.y;
end program StructSample;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    init_compiler(sample_mini.string().c_str());

    enable_flag_verbose(true);

    int rc = yyparse();

    ASSERT_EQ(0, rc);
}


/// @brief Test for array data type
/// @param --gtest_filter=CompilerF.array_test
TEST_F(CompilerF, array_test)
{
    auto ws = create_workspace();

    char const *sample = R"(/* array_test  */
program StructSample:
    declare p array [10] of integer;
    set p[1] := 123;
    output p[1];
end program StructSample;
)";

    auto sample_mini = ws / "sample.mini";
    ASSERT_TRUE(save_as_text(sample, sample_mini));
    ASSERT_TRUE(freopen(sample_mini.string().c_str(), "r", stdin));

    enable_flag_verbose(true);

    init_compiler(sample_mini.string().c_str());
    int rc = yyparse();

    ASSERT_EQ(0, rc);
}

// Local Variables:
// mode: c++
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
