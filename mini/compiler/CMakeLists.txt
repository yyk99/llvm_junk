#
#
#

project(mini_compiler)
cmake_minimum_required(VERSION 3.10)

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

add_executable(compiler
    compiler.cpp
    
    ${FLEX_lexer_OUTPUTS}
    ${BISON_parser_OUTPUTS}
)


find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core mcjit native)


add_executable(gen_samples 
    gen_samples.cpp

)

target_link_libraries(gen_samples ${llvm_libs})
