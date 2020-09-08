#
#
#

project(mini_compiler)
cmake_minimum_required(VERSION 3.10)

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

BISON_TARGET(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
  COMPILE_FLAGS "-t -v"
)
FLEX_TARGET(lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "lexer: ${FLEX_lexer_OUTPUTS}")
message(STATUS "parser: ${BISON_parser_OUTPUTS}")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core mcjit native)

llvm_map_components_to_libnames(llvm_interp_libs
  Core
  ExecutionEngine
  Interpreter
  MC
  MCJIT
  Support
  nativecodegen
)

add_executable(compiler
  compiler.cpp
  parser_bits.cpp
  parser_bits.h
  TreeNode.cpp
  TreeNode.h

  show_type_details.cpp
  llvm_helper.h
    
  ${FLEX_lexer_OUTPUTS}
  ${BISON_parser_OUTPUTS}
  )
target_compile_options(compiler PRIVATE -Wparentheses)
target_link_libraries(compiler ${llvm_libs})

add_executable(gen_samples 
  gen_samples.cpp
  show_type_details.cpp
  llvm_helper.h
  )
target_compile_options(gen_samples PRIVATE -Wparentheses)
target_link_libraries(gen_samples ${llvm_interp_libs})

#
# driver and other scripts
#

set (RTL_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/../lib/bin)
configure_file(mini.sh.config mini.sh @ONLY NEWLINE_STYLE UNIX)

set (TEST_MINI_DIRECTORY ${PROJECT_SOURCE_DIR}/../tests)
configure_file(all-test-compile.sh.config all-test-compile.sh @ONLY NEWLINE_STYLE UNIX)
