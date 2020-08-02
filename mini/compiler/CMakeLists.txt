#
#
#

project(mini_compiler)
cmake_minimum_required(VERSION 3.10)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

BISON_TARGET(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
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
