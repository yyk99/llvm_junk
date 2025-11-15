# Google Test Examples for LLVM Unsorted

This directory contains Google Test unit tests for LLVM examples.

## Setup

### Installing Google Test

**Ubuntu/Debian:**
```bash
sudo apt-get install libgtest-dev
```

**Or using vcpkg:**
See `mini/README.md` for vcpkg setup instructions.

## Building Tests

```bash
cd unsorted
mkdir -p build && cd build
cmake .. -DBUILD_TESTS=ON
cmake --build .
```

## Running Tests

```bash
# Run all tests
ctest

# Run specific test
./bin/test_example

# Run with verbose output
ctest --verbose
```

## Creating New Tests

1. Create a new test file in `test/` directory:
   ```cpp
   // test/test_myfeature.cpp
   #include <gtest/gtest.h>
   #include "llvm/ADT/SmallVector.h"

   TEST(MyFeature, BasicTest) {
       // Your test code here
       EXPECT_EQ(1, 1);
   }
   ```

2. Add it to `test/CMakeLists.txt`:
   ```cmake
   add_gtest_executable(test_myfeature)
   ```

3. Rebuild and run:
   ```bash
   cmake --build .
   ctest
   ```

## Example Tests

- `test_example.cpp` - Demonstrates testing LLVM SmallVector with Google Test

## Reference

- [Google Test Primer](https://google.github.io/googletest/primer.html)
- [LLVM ADT Documentation](https://llvm.org/docs/ProgrammersManual.html)
