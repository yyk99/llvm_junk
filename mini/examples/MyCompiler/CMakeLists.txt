cmake_minimum_required(VERSION 3.20)
project(MyCompiler)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Parser generation (GNU Bison 3.x)
set(PARSER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/MyParser.cc)
set(PARSER_HEADER ${CMAKE_CURRENT_BINARY_DIR}/MyParser.hh)
set(PARSER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/MyParser.y)

add_custom_command(
  OUTPUT ${PARSER_OUTPUT} ${PARSER_HEADER}
  COMMAND ${BISON_EXECUTABLE}
    --defines=${PARSER_HEADER}
    -o ${PARSER_OUTPUT}
    ${PARSER_SOURCE}
  DEPENDS ${PARSER_SOURCE}
  COMMENT "[BISON] Building MyParser from MyParser.y"
)

# Scanner generation (flex C++ mode)
set(SCANNER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/MyScanner.cc)
set(SCANNER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/MyScanner.l)

add_custom_command(
  OUTPUT ${SCANNER_OUTPUT}
  COMMAND ${FLEX_EXECUTABLE}
    -o ${SCANNER_OUTPUT}
    ${SCANNER_SOURCE}
  DEPENDS ${SCANNER_SOURCE}
  COMMENT "[FLEX] Building MyScanner from MyScanner.l"
)

# Mark generated files
set_source_files_properties(
  ${PARSER_OUTPUT}
  ${PARSER_HEADER}
  ${SCANNER_OUTPUT}
  PROPERTIES GENERATED TRUE
)

# Include directories
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Build executable
add_executable(mycompiler
  MyCompiler.cc
  MyScanner.l
  MyParser.y
  ${PARSER_OUTPUT}
  ${SCANNER_OUTPUT}
)

# Compiler options
if (MSVC)
    target_compile_options(mycompiler PRIVATE /wd4244 /wd4267 /wd4624)
    target_compile_definitions(mycompiler PRIVATE -D__STDC_VERSION__=199901)
else()
    target_compile_options(mycompiler PRIVATE -g)
endif()

set_target_properties(mycompiler PROPERTIES FOLDER "Example Apps")

