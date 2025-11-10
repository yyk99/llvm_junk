# Problems Solved

## Migration to GNU Bison 3.x

**Note:** The project has been migrated from bison++ to GNU Bison 3.x. The sections below describe historical issues that were encountered with bison++ before the migration. The project now uses standard GNU Bison 3.x (version 3.0 or later required).

## Incremental Build Issue with bison++ (Historical)

**Problem:**
- CMake was rebuilding everything on every build, even when no files changed
- The root cause: CMake's BISON_TARGET with DEFINES_FILE generates --defines=parser.h which bison++ doesn't support
- This caused CMake to expect parser.hpp but got parser.h, triggering deletion and rebuild every time

**Solution:**
- Replaced BISON_TARGET with a custom add_custom_command
- Uses bison++'s -d flag to generate the header file (creates parser.h alongside parser.cpp)
- Explicitly declares both parser.cpp and parser.h as outputs
- Properly tracks dependency on parser.y only

**Verified:**
- Clean builds work correctly
- No-change rebuilds are instant (no unnecessary compilation)
- Changing parser.y triggers only parser rebuild
- Changing source files rebuilds only affected files

**Commit:** d001149

## CMakeLists.txt for examples/MyCompiler (Historical)

**Created:**
- CMakeLists.txt that mirrors the Makefile functionality:
  - Uses bison++ to generate MyParser.cc and MyParser.h from MyParser.y (now uses GNU Bison 3.x)
  - Uses flex++ to generate MyScanner.cc from MyScanner.l
  - Compiles MyCompiler.cc along with generated files
  - Links everything into a "mycompiler" executable

**Key features:**
- Uses custom commands for bison++ (same approach as main compiler)
- Properly tracks dependencies on .y and .l files
- Marks generated files as GENERATED for proper build tracking
- Includes both source and build directories

**Commit:** 05f0cfe

**Note:** The examples/MyCompiler code has compatibility issues with modern flex++ (flex with C++ mode). Both the original Makefile and the CMakeLists.txt fail to build due to conflicts between the generated scanner code and FlexLexer.h. The example appears to be outdated and would require refactoring to work with current flex/bison++ versions.

The specific issue is that flex++ generates `#define yyFlexLexer yyFlexLexer` which creates recursive definition conflicts with the yyFlexLexer class in FlexLexer.h.

● Perfect! I've successfully fixed the compiler errors in parser_bits.cpp. Here's a summary of the changes:

  Fixed Issues

  1. Opaque Pointer Migration (Line 320)

  - Error: getPointerElementType() no longer exists in LLVM 18
  - Fix: Removed the call since we now pass the struct type directly, not a pointer type

  2. CreateStructGEP Calls (Lines 421, 734)

  - Error: Missing type parameter as first argument
  - Fix: Updated to extract the allocated type from AllocaInst and pass it as the first argument:
  Type *struct_type = nullptr;
  if (auto *AI = dyn_cast<AllocaInst>(sym)) {
      struct_type = AI->getAllocatedType();
  } else {
      struct_type = sym->getType();
  }
  Builder.CreateStructGEP(struct_type, sym, off, "struct_fld");

  3. CreateLoad Call (Line 746)

  - Error: Missing type parameter as first argument
  - Fix: Refactored generate_dot_load to extract the field type from the struct and pass it to CreateLoad:
  StructType *stype = cast<StructType>(struct_type);
  Type *field_type = stype->getElementType(off);
  val = Builder.CreateLoad(field_type, LB, "load_fld");

  4. Additional Fixes

  - Added missing implementation for show_type_details(Type *t) in show_type_details.cpp
  - Removed llvm_Type_dump.cpp and llvm_Value_dump.cpp from CMakeLists.txt (LLVM 18 provides these natively)

● Perfect! All tests now pass. Here's a summary of the fixes:

  Test Build Fixes

  Problem

  The tests were failing to link with undefined reference errors for TheContext and Builder, which were declared as static in
  parser_bits.cpp (internal linkage) but test files were trying to use extern declarations to access them.

  Solution

  Made TheContext and Builder non-static (global linkage) so they can be properly accessed from test files:

  parser_bits.cpp:39
  // Changed from: static LLVMContext TheContext;
  LLVMContext TheContext;

  parser_bits.cpp:181
  // Changed from: static IRBuilder<> Builder(TheContext);
  IRBuilder<> Builder(TheContext);

  Test files (test_symbol_type_table.cpp, test_t2.cpp)
  // Now properly use extern declarations
  extern LLVMContext TheContext;
  extern IRBuilder<> Builder;