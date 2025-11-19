# -*- mode: cmake; fill-column: 80; -*-

cmake_minimum_required(VERSION 3.10)
project(unsorted)

if (POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

option (BUILD_TESTS "Build tests" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Find llc executable (LLVM static compiler)
find_program(LLC_EXECUTABLE
  NAMES llc-${LLVM_VERSION_MAJOR} llc
  HINTS ${LLVM_TOOLS_BINARY_DIR}
  DOC "LLVM static compiler"
)
if(NOT LLC_EXECUTABLE)
  set(LLC_EXECUTABLE "llc")
  message(STATUS "llc executable not found, using fallback: ${LLC_EXECUTABLE}")
else()
  message(STATUS "Found llc: ${LLC_EXECUTABLE}")
endif()

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# add a stand-alone application (a toy)
macro (add_toy_target toy)
  # Now build our tools
  add_executable(${toy} ${toy}.cpp)

  # Find the libraries that correspond to the LLVM components
  # that we wish to use
  # Note: In LLVM 18, MCJIT is deprecated. Using interpreter as fallback.
  llvm_map_components_to_libnames(llvm_libs
    core
    support
    executionengine
    mcjit
    native
    nativecodegen
    interpreter
    orcjit
    runtimedyld
    object
  )

  # Link against LLVM libraries
  target_link_libraries(${toy} ${llvm_libs} extra)
endmacro (add_toy_target)

add_library(extra STATIC Module_dump.cpp)

add_toy_target(example1)
add_toy_target(example2)
add_toy_target(extracting_a_scalar_from_the_vector)
add_toy_target(global_var)
add_toy_target(return_statement)
add_toy_target(function_with_args)
add_toy_target(block_with_expression)
add_toy_target(if_then_else)
add_toy_target(loop)
add_toy_target(memory_access_operations)
add_toy_target(reading_from_memory)
add_toy_target(insert_scalar_into_vector)
add_toy_target(getting_the_address_of_an_element)
add_toy_target(struct_literal_vs_identified)

if (BUILD_TESTS)
  enable_testing()

  # Find GoogleTest using vcpkg or system installation
  find_package(GTest QUIET)

  if(NOT GTest_FOUND)
    message(WARNING "GoogleTest not found. To install:")
    message(WARNING "  Ubuntu/Debian: sudo apt-get install libgtest-dev")
    message(WARNING "  Or use vcpkg: see mini/README.md for vcpkg setup")
    message(WARNING "Skipping Google Test targets...")
    set(GTEST_AVAILABLE FALSE)
  else()
    message(STATUS "Found GoogleTest: ${GTEST_LIBRARIES}")
    set(GTEST_AVAILABLE TRUE)
    # MS VS2012 needs this to compile tuple templates
    add_definitions(-D_VARIADIC_MAX=10)
    include(GoogleTest)
  endif()

  # add a test to run a toy and try to compile its output
  macro (add_toy_target_test t)
    add_test(
      NAME ${t}
      COMMAND bash test_compile.sh ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${t}
    )
  endmacro(add_toy_target_test)

  configure_file(test_compile.sh.config test_compile.sh @ONLY NEWLINE_STYLE UNIX)

  add_test(example1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/example1)
   
  # these tests produce llc input stream
  # run and compile
  add_toy_target_test(example2)
  add_toy_target_test(extracting_a_scalar_from_the_vector)
  add_toy_target_test(global_var)
  add_toy_target_test(return_statement)
  add_toy_target_test(function_with_args)
  add_toy_target_test(block_with_expression)
  add_toy_target_test(if_then_else)
  add_toy_target_test(loop)
  add_toy_target_test(memory_access_operations)
  add_toy_target_test(reading_from_memory)
  add_toy_target_test(insert_scalar_into_vector)
  add_toy_target_test(getting_the_address_of_an_element)
  add_toy_target_test(struct_literal_vs_identified)

  # Macro for creating Google Test executables
  macro(add_gtest_executable test_name)
    if(GTEST_AVAILABLE)
      add_executable(${test_name} ${test_name}.cpp)
      target_link_libraries(${test_name}
        GTest::gtest
        GTest::gtest_main
        ${llvm_libs}
        extra
      )
      gtest_discover_tests(${test_name})
    endif()
  endmacro()

  # Add test subdirectory if it exists and GTest is available
  if(GTEST_AVAILABLE AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test)
    add_subdirectory(test)
  endif()
endif()

# Local Variables:
# mode: cmake
# indent-tabs-mode: nil
# compile-command: "cmake --build --preset debug" 
# End:
