# Mini Compiler

A compiler for the "Mini" programming language that generates LLVM intermediate representation (IR).

## Overview

The Mini language is a Pascal-like programming language with support for:
- Basic types: integer, real, boolean, string
- Arrays and structures
- Functions and procedures
- Control flow: if/then/else, for loops, while loops
- Labeled blocks with break/repeat constructs

The compiler parses Mini source code and generates LLVM IR, which can then be compiled to native executables.

## Required Tools

### Core Build Tools
- **CMake** 3.10 or later (3.19+ recommended for CMake presets)
- **C++ compiler** with C++11 support (GCC, Clang)
- **Make** or other CMake-supported build system

### Parser/Lexer Generation
- **bison++** - C++ parser generator (**NOT** standard GNU Bison)
  - The parser.y file uses bison++ specific directives (`%name`, `%header{`)
  - Standard GNU Bison will not work with this grammar file
  - Tested with version 1.21.9-1
  - Install: `apt-get install bison++` (Debian/Ubuntu) or equivalent
- **Flex** 2.6+ - Lexical analyzer generator
  - Tested with version 2.6.4

### Compiler Backend
- **LLVM** development libraries
  - Tested with LLVM 18.x
  - Required components: Core, ExecutionEngine, Interpreter, MC, MCJIT, Support, nativecodegen
  - Install: `apt-get install llvm-dev` (Debian/Ubuntu) or equivalent

### Testing (Optional)
- **vcpkg** - C++ package manager (for GoogleTest)
  - Used to manage GoogleTest dependency
  - Install: Follow [vcpkg installation guide](https://github.com/microsoft/vcpkg#quick-start-unix)
  - GoogleTest installed automatically via vcpkg manifest when building with `BUILD_TESTS=ON`

## Installation

### Debian/Ubuntu

```bash
sudo apt-get update
sudo apt-get install cmake build-essential
sudo apt-get install bison++ flex
sudo apt-get install llvm-dev
```

### Other Systems

Ensure you have:
- bison++ (not bison)
- flex
- LLVM development headers and libraries
- CMake and a C++ compiler

## Building

### Prerequisites for Testing

If you want to build with tests enabled (`BUILD_TESTS=ON`), you need to set up vcpkg:

```bash
# Clone vcpkg (if not already installed)
cd ~
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh

# Update CMakePresets.json to point to your vcpkg installation
# The preset expects vcpkg at: $HOME/src/vcpkg
# If your vcpkg is elsewhere, update CMAKE_TOOLCHAIN_FILE in CMakePresets.json
```

The project's `vcpkg.json` manifest will automatically install GoogleTest when CMake configures.

### Using CMake Presets (Recommended)

```bash
# Configure (GoogleTest installed automatically via vcpkg if BUILD_TESTS=ON)
cmake --preset debug

# Build
cmake --build --preset debug

# Run tests
ctest --preset debug

# Install
cmake --build --preset debug --target install
```

Available presets:
- `debug` - Debug build with tests
- `release` - Release build without tests
- `debug-no-tests` - Debug without tests (faster build)
- `release-with-tests` - Release with tests

### Traditional Build

```bash
mkdir -p build && cd build
cmake ..
make
make install
```

### Build Options

- `BUILD_TESTS` - Build unit tests (default: ON)
  - **Note**: Requires vcpkg to be set up (see Prerequisites for Testing)
  - If vcpkg is not available, use `BUILD_TESTS=OFF` or `--preset debug-no-tests`
- `CMAKE_INSTALL_PREFIX` - Installation directory (default: ~/.local)
- `CMAKE_BUILD_TYPE` - Build type: Debug or Release (default: Debug)

Example:
```bash
cmake --preset debug -DCMAKE_INSTALL_PREFIX=/usr/local
```

## Testing

### Running All Tests

```bash
# With CMake presets
ctest --preset debug

# Traditional
cd build && ctest
```

### Running Individual Tests

```bash
# Unit tests
./build/debug/compiler/test/test_t1

# Compile all Mini test programs
./build/debug/compiler/all-test-compile.sh
```

Test programs are located in the `tests/` directory with `.mini` extension.

## Usage

### Compiling Mini Programs

The `mini` wrapper script handles the complete compilation pipeline:

```bash
# Install the compiler first
cmake --build --preset debug --target install

# Compile a Mini program
mini tests/hello_world.mini

# Run the executable
./hello_world
```

### Manual Compilation Pipeline

```bash
# 1. Generate LLVM IR
compiler tests/hello_world.mini > hello_world.ll

# 2. Compile IR to assembly
llc -O=0 -o hello_world.s hello_world.ll

# 3. Link with runtime library
cc -g -no-pie -o hello_world hello_world.s -L~/.local/lib -lmini
```

## Example Mini Program

```mini
program Foo:
    declare (hello, good_bye) string;

    set hello := "Hello world";
    set good_bye := "Good Bye...";

    output hello;
    output good_bye;
    output 1, 42, 123;
end program Foo;
```

## Project Structure

```
.
├── compiler/           # Main compiler source
│   ├── parser.y        # Bison++ grammar file
│   ├── lexer.l         # Flex lexer definition
│   ├── compiler.cpp    # Main compiler driver
│   ├── TreeNode.{h,cpp} # AST node classes
│   ├── parser_bits.{h,cpp} # Code generation
│   └── test/           # Unit tests
├── lib/                # Runtime library (C)
│   ├── rtl_output*.c   # Output functions
│   └── rtl_*.c         # Runtime utilities
├── tests/              # Mini language test programs
│   └── errors/         # Error test cases
└── examples/           # Example projects
```

## Troubleshooting

### "Unknown directive %name" or "%header"

You are using standard GNU Bison instead of bison++. Install bison++ package:
```bash
sudo apt-get install bison++
```

### "Could not find LLVM"

Install LLVM development packages:
```bash
sudo apt-get install llvm-dev
```

### Tests fail to build

Disable tests if you only need the compiler:
```bash
cmake --preset debug-no-tests
```

### "Could NOT find GTest"

vcpkg is not set up or the toolchain file path is incorrect:
```bash
# Verify vcpkg is installed
ls $HOME/src/vcpkg/scripts/buildsystems/vcpkg.cmake

# If vcpkg is in a different location, update CMakePresets.json:
# Change CMAKE_TOOLCHAIN_FILE to point to your vcpkg installation
```

Alternatively, install GoogleTest system-wide and remove the vcpkg toolchain:
```bash
sudo apt-get install libgtest-dev
# Then comment out CMAKE_TOOLCHAIN_FILE in CMakePresets.json
```

### vcpkg conflicts with system LLVM

This should not happen with the current configuration, as the project is set up to:
- Use system LLVM (bypassing vcpkg)
- Only use vcpkg for GoogleTest
- Create necessary aliases for LLVM's dependencies

If you encounter issues, ensure LLVM was found before vcpkg by checking CMake output:
```
-- Found LLVM 18.1.3
-- Using LLVMConfig.cmake in: /usr/lib/llvm-18/cmake
```

## License

[License information to be added]

## Contributing

[Contributing guidelines to be added]
