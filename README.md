# LLVM Bits and Pieces

A collection of LLVM learning materials, examples, and experimental projects.

## Contents

- **mini/** - Complete compiler for the MINI programming language (Pascal-like) that generates LLVM IR
- **examples/** - LLVM tutorial examples (Fibonacci, Kaleidoscope, BrainF, JIT demos)
- **toy/** - Toy language implementations from LLVM tutorials
- **hello/** - Simple C programs demonstrating LLVM IR generation
- **unsorted/** - Standalone examples of LLVM IR builder patterns (loops, memory ops, functions)

## Requirements

- LLVM development libraries (tested with LLVM 18.x)
- CMake 3.10+
- C++ compiler with C++11 support

For Mini compiler specifically:
- GNU Bison 3.x (version 3.0 or later)
- Flex 2.6+

## Quick Start

Each directory is an independent project with its own build system.

### Mini Compiler

```bash
cd mini
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

See mini/README.md for detailed instructions.

### LLVM Examples

```bash
cd examples
mkdir -p build && cd build
cmake ..
make
```

### Hello Examples

```bash
cd hello
make
```

## Documentation

- CLAUDE.md - Guide for AI assistants working with this repository
- mini/README.md - Comprehensive Mini compiler documentation
- mini/CLAUDE.md - Mini compiler architecture and development guide
