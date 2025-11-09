# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This is a collection of LLVM learning materials, examples, and experimental projects. The repository contains multiple independent subdirectories with different purposes and build systems.

## Directory Structure

### mini/
A complete compiler for the "Mini" programming language (Pascal-like) that generates LLVM IR.

**Build system**: CMake with presets support
**Language**: C++ with Flex/Bison++ (NOT standard GNU Bison)
**Documentation**: See `mini/CLAUDE.md` and `mini/README.md` for detailed build instructions

Key commands:
```bash
cd mini
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

The Mini compiler has comprehensive documentation in its own directory. Refer to `mini/CLAUDE.md` for architecture details and development guidelines.

### examples/
LLVM example programs demonstrating various LLVM features:
- **Fibonacci** - JIT execution example
- **Kaleidoscope** - Tutorial language implementation (multiple chapters)
- **BrainF** - Brainf*ck interpreter
- **ExceptionDemo** - Exception handling demonstration
- **HowToUseJIT** - JIT compilation examples
- **ModuleMaker** - Basic module creation
- **ParallelJIT** - Parallel JIT compilation

**Build system**: CMake (independent from root)
**Language**: C++

Build from `examples/` directory:
```bash
cd examples
mkdir -p build && cd build
cmake ..
make
```

Most examples follow the pattern from Kaleidoscope tutorial chapters, showing progressive feature additions.

### toy/
Toy language implementations based on LLVM tutorials.

**Build system**: None (manual compilation)
**Language**: C++

Compile manually (see `toy/README.txt`):
```bash
cd toy
clang++ -g -O3 toy3.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o toy3
```

Different toy*.cpp files represent different stages/chapters of the tutorial.

### hello/
Simple C programs demonstrating LLVM IR generation using Clang.

**Build system**: Makefile
**Language**: C

```bash
cd hello
make
```

These are basic examples showing how C code translates to LLVM IR. The Makefile generates `.ll` (LLVM IR) files from `.c` files using Clang.

### unsorted/
Standalone C++ examples demonstrating specific LLVM IR builder patterns:
- Control flow (if/then/else, loops)
- Memory operations (load/store, arrays)
- Functions and calling conventions
- Vector operations
- Global variables
- Block expressions

**Build system**: CMake (when built)
**Language**: C++

These are educational snippets showing how to use LLVM IR builder API for specific constructs. Each file is typically self-contained.

## Build Requirements

### Core Dependencies
- **LLVM development libraries** (tested with LLVM 18.x)
  - Required components: Core, ExecutionEngine, Interpreter, MC, MCJIT, Support, nativecodegen
  - Install: `apt-get install llvm-dev` (Debian/Ubuntu)

- **CMake** 3.10+ (3.19+ recommended for preset support)

- **C++ compiler** with C++11 support (GCC or Clang)

### Mini-Specific Dependencies
The `mini/` subdirectory requires additional tools:
- **bison++** (NOT standard GNU Bison) - C++ parser generator
- **Flex** 2.6+ - Lexical analyzer generator
- **vcpkg** (optional) - For GoogleTest when building with tests

See `mini/README.md` for detailed setup instructions.

## Building the Repository

**Important**: There is NO root-level CMakeLists.txt. Each subdirectory is an independent project.

To build any component:
1. Navigate to the specific subdirectory
2. Follow the build instructions for that component
3. Each component has its own build system and dependencies

Example workflow:
```bash
# Build the Mini compiler
cd mini
cmake --preset debug
cmake --build --preset debug

# Build LLVM examples (separate build)
cd ../examples
mkdir -p build && cd build
cmake ..
make

# Build hello examples
cd ../../hello
make
```

## Development Notes

### LLVM Version Compatibility
The code in this repository has been migrated to work with modern LLVM (18.x):
- Uses opaque pointers (not typed pointers)
- All IR builder calls include explicit type parameters
- Compatible with LLVM's current API design

If working with older LLVM versions, some API adjustments may be needed.

### Mini Compiler Development
For the Mini compiler, always:
- Use bison++ (not GNU bison) - the parser.y file uses bison++-specific syntax
- Build with CMake presets when possible
- Run tests with `ctest --preset debug`
- See `mini/CLAUDE.md` for detailed architecture and development guidelines

### Common LLVM Patterns
The `unsorted/` directory contains useful reference examples for:
- Creating functions, basic blocks, and control flow
- Memory operations (alloca, load, store, GEP)
- Working with arrays, vectors, and aggregates
- Setting up JIT execution
- Module creation and IR dumping

These examples are valuable references when implementing new LLVM-based features.

## Git Workflow

The repository uses standard Git workflow. Recent commits show:
- Migration to modern LLVM APIs
- Addition of comprehensive documentation
- Improved build system with CMake presets

### Commit Message Conventions

When creating commits for this repository:
- Use ASCII-only characters (no emojis or Unicode symbols)
- Use '-' symbol for bullet points in list items
- Keep messages clear and descriptive
- Follow the format: brief summary, then detailed explanation if needed
