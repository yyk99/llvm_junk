#
#
#

cmake_minimum_required(VERSION 3.10)
project(examples_flex)

find_package(FLEX)

FLEX_TARGET(lexer lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
#    COMPILE_FLAGS "-+" 
)

message(STATUS "FLEX_lexer_OUTPUTS=${FLEX_lexer_OUTPUTS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(scanner
    scanner.cpp
    scanner.h
    ${FLEX_lexer_OUTPUTS}
)

message(STATUS "FLEX_LIBRARIES=${FLEX_LIBRARIES}")
set(FLEX_LIBRARIES "-lfl")
target_link_libraries(scanner ${FLEX_LIBRARIES} )
