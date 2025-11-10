# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

For known build issues and their solutions, see [PROBLEMS_SOLVED.md](PROBLEMS_SOLVED.md).

## Project Overview

This is a compiler for the "Mini" programming language that compiles to LLVM IR. The compiler is built using Flex (lexer) and Bison (parser), and generates LLVM intermediate representation which is then compiled to native code.

The Mini language supports:
- Basic types: integer, real, boolean, string
- Arrays and structures
- Functions and procedures
- Control flow: if/then/else, for loops, while loops
- Labeled blocks with break/repeat constructs

## Build System

The project uses CMake with a two-level build structure (root and compiler subdirectories).

### Building with CMake Presets (Recommended)

CMake presets simplify the build configuration. Requires CMake 3.19+.

```bash
# List available presets
cmake --list-presets

# Configure with a preset
cmake --preset debug          # Debug build with tests
cmake --preset release        # Release build without tests
cmake --preset debug-no-tests # Debug without tests (faster)

# Build
cmake --build --preset debug

# Run tests
ctest --preset debug
```

Available presets:
- `debug` - Debug build with tests (default)
- `release` - Optimized release build without tests
- `debug-no-tests` - Debug build without tests for faster compilation
- `release-with-tests` - Release build with tests enabled

You can customize settings in `CMakeUserPresets.json` (gitignored) without modifying the shared `CMakePresets.json`.

### Building the traditional way

```bash
# From repository root
mkdir -p build && cd build
cmake ..
make
```

### Installing the compiler

```bash
# With CMake presets
cmake --build --preset debug --target install

# Traditional way - from build directory
make install

# Default install location: ~/.local
```

The install creates:
- `~/.local/bin/compiler` - The Mini compiler executable
- `~/.local/bin/mini` - Wrapper script to compile and link Mini programs
- `~/.local/lib/libmini.a` - Runtime library

### Build configuration

- Default build type is Debug
- Tests are enabled by default (`BUILD_TESTS=ON`)
- GoogleTest is installed via vcpkg manifest mode during the build
- LLVM must be installed and findable by CMake

### Required tools

**IMPORTANT**: This project requires **GNU Bison 3.x** (version 3.0 or later).

The `parser.y` file uses modern GNU Bison 3.x features and syntax. Older versions of Bison (2.x or earlier) will not work with this grammar file and will fail with syntax errors.

To check your Bison version:
```bash
bison --version
```

Other required tools:
- Flex 2.6+ (lexer generator)
- LLVM development libraries (tested with LLVM 18.x)
- CMake 3.10+ (3.19+ for preset support)
- C++ compiler with C++11 support

## Running Tests

### Unit tests (C++)

```bash
# With CMake presets
ctest --preset debug

# Traditional way - from build directory
ctest

# Or run individual test executables
./build/debug/compiler/test/test_t1  # With presets
./compiler/test/test_t1               # Traditional
```

Unit tests are in `compiler/test/*.cpp` and use GoogleTest. The CMake macro `make_test_executables()` automatically creates test executables from all .cpp files in the test directory.

### Mini language tests

The repository includes Mini language test programs in `tests/` directory:

```bash
# From build directory
./compiler/all-test-compile.sh
```

This runs the compiler against all `*.mini` files in the tests directory.

Error tests are in `tests/errors/` and verify the compiler correctly reports errors.

## Compiling Mini Programs

### Using the wrapper script (recommended)

```bash
mini tests/hello_world.mini
# Creates executable: hello_world
./hello_world
```

The `mini` wrapper script:
1. Runs `compiler` to generate LLVM IR (.ll file)
2. Runs `llc` to compile IR to assembly (.s file)
3. Runs `cc` to link with runtime library and create executable

### Manual compilation

```bash
# Generate LLVM IR
compiler tests/hello_world.mini > hello_world.ll

# Compile to assembly
llc -O=0 -o hello_world.s hello_world.ll

# Link with runtime library
cc -g -no-pie -o hello_world hello_world.s -L~/.local/lib -lmini
```

## Architecture

### Compiler Pipeline

1. **Lexer** (`lexer.l`) - Tokenizes input using Flex
2. **Parser** (`parser.y`) - Parses tokens into AST using Bison
3. **AST** (`TreeNode.h/cpp`) - Tree node classes representing parsed constructs
4. **Code Generation** (`parser_bits.cpp`) - Generates LLVM IR from AST
5. **Output** - LLVM IR emitted to stdout

### Key Components

- **TreeNode classes**: Abstract syntax tree nodes
  - `TreeIdentNode` - Identifiers
  - `TreeNumericalNode` / `TreeDNumericalNode` - Integer/Real literals
  - `TreeTextNode` - String literals
  - `TreeBooleanNode` - Boolean literals
  - `TreeBinaryNode` / `TreeUnaryNode` - Operators

- **Symbol table**: Managed in `parser_bits.cpp`
  - `symbols_insert()` / `symbols_find()` - Variable symbols
  - `symbols_insert_function()` / `symbols_find_function()` - Function symbols
  - `symbols_push()` / `symbols_pop()` - Scope management

- **LLVM code generation**: In `parser_bits.cpp`
  - `generate_expr()` - Generate IR for expressions
  - `generate_load()` - Generate load instructions
  - `generate_alloca()` - Generate stack allocations
  - Array type handling with `CreateArrayType()` and related functions

- **Runtime library** (`lib/`): C functions for I/O and built-ins
  - `rtl_output.c` - Output for various types
  - `rtl_allocate_array.c` - Dynamic array allocation
  - `rtl_fix.c` - Type conversions

### Parser Structure

The Bison parser (`parser.y`) defines:
- Grammar rules for Mini language syntax
- Actions that build AST nodes
- Integration with LLVM code generation via `parser_bits.h` functions

The parser uses semantic actions to call code generation functions directly during parsing, creating LLVM IR incrementally rather than building a complete AST first.

## Development Notes

### Commit Message Conventions

When creating commit messages for this repository:
- Use ASCII-only characters (no emojis or Unicode symbols)
- Use '-' symbol for bullet points in list items
- Keep messages clear and descriptive

Example:
```
Add feature X to improve Y

Changes:
- Implement new functionality in module A
- Update tests to cover edge cases
- Refactor helper functions for clarity
```

### Compiler Options

The `compiler` executable accepts:
- `-d` - Enable parser debug output (requires YYDEBUG)
- `-v` - Verbose error reporting (sets `flag_verbose`)
- Input file as positional argument (defaults to stdin)

### Generated Files

The build process generates:
- `build/compiler/parser.cpp` / `parser.h` - From `parser.y`
- `build/compiler/lexer.cpp` - From `lexer.l`
- `build/compiler/mini.sh` - From `mini.sh.config` (template substitution)
- `build/compiler/all-test-compile.sh` - From `all-test-compile.sh.config`

Do not edit generated files directly - modify the source `.y`, `.l`, or `.config` files.

### LLVM Integration

The compiler uses LLVM components:
- Core - Basic LLVM types and IR
- MCJIT - JIT compilation support
- Native - Native code generation

LLVM version must be compatible with the API used in the codebase.
