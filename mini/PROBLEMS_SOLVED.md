# Problems Solved

## Incremental Build Issue with bison++

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

## CMakeLists.txt for examples/MyCompiler

**Created:**
- CMakeLists.txt that mirrors the Makefile functionality:
  - Uses bison++ to generate MyParser.cc and MyParser.h from MyParser.y
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
