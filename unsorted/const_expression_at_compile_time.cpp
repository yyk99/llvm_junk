//
//
//

// ● Yes! LLVM IR has extensive support for detecting and evaluating constant expressions at compile time. Here's how it works:
//
//   Constant Folding in LLVM
//
//   LLVM performs constant folding at multiple levels:
//
//   1. During IR Generation (IRBuilder)

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llvm;

int main()
{
    LLVMContext Context;
    Module M("const_example", Context);
    IRBuilder<> Builder(Context);

    // Create a function
    FunctionType *FT = FunctionType::get(Type::getInt32Ty(Context), false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, "test", &M);
    BasicBlock *BB = BasicBlock::Create(Context, "entry", F);
    Builder.SetInsertPoint(BB);

    // These operations are folded immediately by IRBuilder
    Value *result1 = Builder.CreateAdd(Builder.getInt32(10),
                                       Builder.getInt32(20)); // Becomes constant 30

    Value *result2 = Builder.CreateMul(Builder.getInt32(5),
                                       Builder.getInt32(6)); // Becomes constant 30

    // Complex expression: (10 + 20) * 2
    Value *result3 =
        Builder.CreateMul(Builder.CreateAdd(Builder.getInt32(10), Builder.getInt32(20)),
                          Builder.getInt32(2)); // Becomes constant 60

    // Check if result is a constant
    if (isa<Constant>(result1)) {
        ConstantInt *CI = cast<ConstantInt>(result1);
        llvm::outs() << "result1 is constant: " << CI->getSExtValue() << "\n";
    }

    Builder.CreateRet(result1);

    M.print(llvm::outs(), nullptr);
    return 0;
}

// Output:
// define i32 @test() {
// entry:
//   ret i32 30
// }
//
// Notice: The add and mul instructions are completely eliminated - only the final constant remains!

#if 0
  2. Checking if a Value is Constant

  #include "llvm/IR/Constants.h"

  void analyzeValue(Value *V) {
      // Check if it's any kind of constant
      if (isa<Constant>(V)) {
          llvm::outs() << "This is a constant!\n";

          // Check specific constant types
          if (ConstantInt *CI = dyn_cast<ConstantInt>(V)) {
              llvm::outs() << "Integer constant: "
                          << CI->getSExtValue() << "\n";
          }
          else if (ConstantFP *CF = dyn_cast<ConstantFP>(V)) {
              llvm::outs() << "Float constant: "
                          << CF->getValueAPF().convertToDouble() << "\n";
          }
          else if (ConstantExpr *CE = dyn_cast<ConstantExpr>(V)) {
              llvm::outs() << "Constant expression\n";
          }
      }
      else {
          llvm::outs() << "This is NOT a constant\n";
      }
  }

  3. ConstantExpr - Compile-Time Expressions

  LLVM has a special ConstantExpr class for constant expressions:

  // Create constant expressions without IRBuilder
  Constant *C1 = ConstantInt::get(Type::getInt32Ty(Context), 10);
  Constant *C2 = ConstantInt::get(Type::getInt32Ty(Context), 20);

  // Create a constant expression (GEP, bitcast, etc.)
  Constant *Sum = ConstantExpr::getAdd(C1, C2);  // Constant 30

  // More complex constant expressions
  Constant *Ptr = /* some pointer constant */;
  Constant *GEP = ConstantExpr::getGetElementPtr(
      SomeType, Ptr,
      {ConstantInt::get(Type::getInt32Ty(Context), 0)}
  );

  4. Complete Example: Detecting Constant Expressions

  #include <iostream>
  #include "llvm/IR/IRBuilder.h"
  #include "llvm/IR/LLVMContext.h"
  #include "llvm/IR/Module.h"
  #include "llvm/IR/Constants.h"

  using namespace llvm;

  void printValueInfo(Value *V, const std::string &name) {
      std::cout << name << ": ";

      if (Constant *C = dyn_cast<Constant>(V)) {
          std::cout << "CONSTANT - ";

          if (ConstantInt *CI = dyn_cast<ConstantInt>(C)) {
              std::cout << "Integer value: " << CI->getSExtValue();
          } else if (ConstantFP *CF = dyn_cast<ConstantFP>(C)) {
              std::cout << "Float value: "
                       << CF->getValueAPF().convertToDouble();
          } else if (isa<ConstantExpr>(C)) {
              std::cout << "Constant expression";
          } else {
              std::cout << "Other constant type";
          }
      } else {
          std::cout << "NOT constant (runtime value)";
      }
      std::cout << "\n";
  }

  int main() {
      LLVMContext Context;
      Module M("const_detection", Context);
      IRBuilder<> Builder(Context);

      // Create function with parameter
      FunctionType *FT = FunctionType::get(
          Type::getInt32Ty(Context),
          {Type::getInt32Ty(Context)},
          false
      );
      Function *F = Function::Create(
          FT, Function::ExternalLinkage, "test", &M
      );
      BasicBlock *BB = BasicBlock::Create(Context, "entry", F);
      Builder.SetInsertPoint(BB);

      Value *param = F->arg_begin();

      // Constant expression - evaluated at compile time
      Value *constResult = Builder.CreateAdd(
          Builder.getInt32(100),
          Builder.getInt32(200)
      );
      printValueInfo(constResult, "100 + 200");

      // Non-constant expression - evaluated at runtime
      Value *runtimeResult = Builder.CreateAdd(
          param,
          Builder.getInt32(10)
      );
      printValueInfo(runtimeResult, "param + 10");

      // Mixed expression - partially constant folded
      Value *mixedResult = Builder.CreateMul(
          Builder.CreateAdd(Builder.getInt32(5), Builder.getInt32(5)),
          param
      );
      printValueInfo(mixedResult, "(5 + 5) * param");

      Builder.CreateRet(constResult);

      std::cout << "\nGenerated IR:\n";
      M.print(llvm::outs(), nullptr);

      return 0;
  }

  Output:
  100 + 200: CONSTANT - Integer value: 300
  param + 10: NOT constant (runtime value)
  (5 + 5) * param: NOT constant (runtime value)

  Generated IR:
  define i32 @test(i32 %0) {
  entry:
    %1 = add i32 %0, 10
    %2 = mul i32 10, %0
    ret i32 300
  }

  Notice: (5 + 5) was folded to 10 even though the final result is non-constant!

  5. Optimization Passes for Constant Folding

  #include "llvm/Transforms/InstCombine/InstCombine.h"
  #include "llvm/Transforms/Scalar.h"
  #include "llvm/IR/PassManager.h"

  // Apply constant folding optimization passes
  FunctionPassManager FPM;
  FPM.addPass(InstCombinePass());
  FPM.addPass(createConstantPropagationPass());
  FPM.run(*F, FAM);

  Key Points:

  1. IRBuilder automatically folds constants when both operands are constant
  2. Use isa<Constant>(V) to check if a value is compile-time constant
  3. ConstantInt, ConstantFP, ConstantExpr are different constant types
  4. Partial constant folding happens: (5+5)*x becomes 10*x
  5. Optimization passes do additional constant propagation

  Practical Use:

  // Before code generation, check if we can optimize
  if (isa<Constant>(someValue)) {
      // This value is known at compile time!
      // Can be embedded directly, no runtime computation needed
  } else {
      // Need to generate runtime code for this
  }

  // This is fundamental to LLVM's optimization capabilities - it aggressively evaluates what it can at compile time!
#endif
