//
// https://riptutorial.com/llvm/example/29450/compilation-of-a-simple-function-in-llvm-4-0
//

#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/Support/TargetSelect.h"

// Optimizations
#include "llvm/Transforms/Scalar.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"

using namespace llvm;

llvm::Function *createSumFunction(Module *module)
{
    /* Builds the following function:

    int sum(int a, int b) {
        int sum1 = 1 + 1;
        int sum2 = sum1 + a;
        int result = sum2 + b;
        return result;
    }
    */

    LLVMContext &context = module->getContext();
    IRBuilder<> builder(context);

    // Define function's signature
    std::vector<Type *> Integers(2, builder.getInt32Ty());
    auto *funcType = FunctionType::get(builder.getInt32Ty(), Integers, false);

    // create the function "sum" and bind it to the module with ExternalLinkage,
    // so we can retrieve it later
    auto *fooFunc = Function::Create(funcType, Function::ExternalLinkage, "sum", module);

    // Define the entry block and fill it with an appropriate code
    auto *entry = BasicBlock::Create(context, "entry", fooFunc);
    builder.SetInsertPoint(entry);

    // Add constant to itself, to visualize constant folding
    Value *constant = ConstantInt::get(builder.getInt32Ty(), 0x1);
    auto *sum1 = builder.CreateAdd(constant, constant, "sum1");

    // Retrieve arguments and proceed with further adding...
    auto args = fooFunc->arg_begin();
    Value *arg1 = &(*args);
    args = std::next(args);
    Value *arg2 = &(*args);
    auto *sum2 = builder.CreateAdd(sum1, arg1, "sum2");
    auto *result = builder.CreateAdd(sum2, arg2, "result");

    // ...and return
    builder.CreateRet(result);

    // Verify at the end
    verifyFunction(*fooFunc);
    return fooFunc;
}

int main(int argc, char *argv[])
{
    // Initilaze native target and execution engine
    llvm::TargetOptions Opts;
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    LLVMLinkInInterpreter(); // Link in the interpreter

    LLVMContext context;
    auto myModule = std::make_unique<Module>("My First JIT", context);
    auto *module = myModule.get();

    // Create execution engine
    std::string err_str; // The string to hold the error message
    llvm::EngineBuilder factory(std::move(myModule));
    factory.setTargetOptions(Opts);
    factory.setErrorStr(&err_str);
    auto executionEngine = std::unique_ptr<llvm::ExecutionEngine>(factory.create());
    if (!executionEngine) {
        std::cerr << "FATAL: " << err_str << "\n";
        exit(1);
    }
    module->setDataLayout(executionEngine->getDataLayout());

    // Create optimizations, not necessary, whole block can be ommited.
    // auto fpm = llvm::make_unique<legacy::FunctionPassManager>(module);
    // fpm->add(llvm::createBasicAAWrapperPass());
    // fpm->add(llvm::createPromoteMemoryToRegisterPass());
    // fpm->add(llvm::createInstructionCombiningPass());
    // fpm->add(llvm::createReassociatePass());
    // fpm->add(llvm::createNewGVNPass());
    // fpm->add(llvm::createCFGSimplificationPass());
    // fpm->doInitialization();

    auto *func = createSumFunction(module); // create function
    executionEngine->finalizeObject(); // compile the module
#if 0
    module->dump();                          // print the compiled code
#else
    module->print(llvm::outs(), nullptr);
#endif

    // Execute using the interpreter's runFunction method
    int arg1 = 5;
    int arg2 = 7;

    std::vector<llvm::GenericValue> args(2);
    args[0].IntVal = llvm::APInt(32, arg1);
    args[1].IntVal = llvm::APInt(32, arg2);

    llvm::GenericValue result = executionEngine->runFunction(func, args);

    std::cout << arg1 << " + " << arg2 << " + 1 + 1 = " << result.IntVal.getSExtValue() << std::endl;

    return 0;
}
// It should work fine when compiled with clang++-4.0 with following flags:
//
// $ llvm-config-4.0 --cxxflags --libs core
