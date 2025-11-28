/*
  Here's a simple example of an LLVM identified struct:
*/

#include "test_llvm_basic.h"

#include <vector>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

static LLVMContext Context;

class test_struct_literal_vs_identified : public test_llvm_basic {
public:
};

TEST_F(test_struct_literal_vs_identified, create_identified_struct)
{
    Module M("identified_struct_example", Context);
    IRBuilder<> Builder(Context);

    // Create an identified struct type named "Point"
    // Identified structs are created with StructType::create() and given a name
    StructType *PointType = StructType::create(Context, "Point");

    ASSERT_FALSE(PointType->isLiteral());

    // Define the body: struct Point { i32 x, i32 y }
    PointType->setBody({
        Type::getInt32Ty(Context), // x coordinate
        Type::getInt32Ty(Context) // y coordinate
    });

    // Create a function that uses this struct
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(Context), false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, "test", &M);
    BasicBlock *BB = BasicBlock::Create(Context, "entry", F);
    Builder.SetInsertPoint(BB);

    // Allocate a Point on the stack
    AllocaInst *point = Builder.CreateAlloca(PointType, nullptr, "p");

    // Get pointer to x field (index 0)
    Value *xPtr = Builder.CreateStructGEP(PointType, point, 0, "x_ptr");
    Builder.CreateStore(Builder.getInt32(10), xPtr);

    // Get pointer to y field (index 1)
    Value *yPtr = Builder.CreateStructGEP(PointType, point, 1, "y_ptr");
    Builder.CreateStore(Builder.getInt32(20), yPtr);

    // Load x value
    Value *xVal = Builder.CreateLoad(Type::getInt32Ty(Context), xPtr, "x_val");

    // Return x
    Builder.CreateRet(xVal);

    // Print the module
    M.print(outs(), nullptr);
}

TEST_F(test_struct_literal_vs_identified, create_literal_struct)
{
    Module M("identified_struct_example", Context);
    IRBuilder<> Builder(Context);

    StructType *PointType =
        StructType::get(Context, {Type::getInt32Ty(Context), Type::getInt32Ty(Context)});

    ASSERT_TRUE(PointType->isLiteral());
    M.dump();
}

/*
  Output:
  ; ModuleID = 'identified_struct_example'

  %Point = type { i32, i32 }

  define i32 @test() {
  entry:
    %p = alloca %Point, align 8
    %x_ptr = getelementptr inbounds %Point, ptr %p, i32 0, i32 0
    store i32 10, ptr %x_ptr, align 4
    %y_ptr = getelementptr inbounds %Point, ptr %p, i32 0, i32 1
    store i32 20, ptr %y_ptr, align 4
    %x_val = load i32, ptr %x_ptr, align 4
    ret i32 %x_val
  }

  Key Differences: Identified vs Literal Structs

  Identified Struct (with name):
  StructType *PointType = StructType::create(Context, "Point");
  PointType->setBody({Type::getInt32Ty(Context), Type::getInt32Ty(Context)});
  // Output: %Point = type { i32, i32 }

  Literal Struct (anonymous):
  StructType *PointType = StructType::get(Context, {
      Type::getInt32Ty(Context),
      Type::getInt32Ty(Context)
  });
  // Output: { i32, i32 } (no name, defined inline)

  Benefits of Identified Structs

  1. Named type - easier to read IR
  2. Can be recursive - struct can contain pointer to itself
  3. Reusable - can reference by name throughout module
  4. Better for complex types - like classes, closures, etc.

  Compile and Run

  clang++ -g identified_struct.cpp \
      $(llvm-config --cxxflags --ldflags --system-libs --libs core) \
      -o identified_struct
  ./identified_struct

  This example shows the fundamental pattern used in LLVM for creating named struct types, like the
  %class.anon closure type we used in the lambda test!
*/
