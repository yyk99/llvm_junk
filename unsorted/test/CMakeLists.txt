# Google Test targets for unsorted examples

# Test for test_llvm_basic utility class
add_gtest_executable(test_llvm_basic_test)

# Example test demonstrating Google Test with LLVM
add_gtest_executable(test_example)

# Based on corresponding global_var.cpp
add_gtest_executable(test_global_var)

# Based on corresponding function_with_args.cpp
add_gtest_executable(test_function_with_args)

# Based on corresponding function_with_args.cpp
add_gtest_executable(test_function_nested)

