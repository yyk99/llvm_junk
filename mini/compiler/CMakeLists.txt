#
#
#

project(mini_compiler)
cmake_minimum_required(VERSION 3.10)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "~/.local" CACHE PATH "..." FORCE)
endif()

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "...")

#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Use custom command for bison++ which doesn't support --defines= syntax
set(PARSER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
set(PARSER_HEADER ${CMAKE_CURRENT_BINARY_DIR}/parser.h)
set(PARSER_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/parser.y)

add_custom_command(
  OUTPUT ${PARSER_OUTPUT} ${PARSER_HEADER}
  COMMAND ${BISON_EXECUTABLE}
    -t -v -d
    -o ${PARSER_OUTPUT}
    ${PARSER_SOURCE}
  DEPENDS ${PARSER_SOURCE}
  COMMENT "[BISON][parser] Building parser with bison ${BISON_VERSION}"
)

FLEX_TARGET(lexer
  ${CMAKE_CURRENT_SOURCE_DIR}/lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

# Mark generated files as GENERATED to ensure proper dependency tracking
set_source_files_properties(
  ${PARSER_OUTPUT}
  ${PARSER_HEADER}
  ${FLEX_lexer_OUTPUTS}
  PROPERTIES GENERATED TRUE
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "lexer: ${FLEX_lexer_OUTPUTS}")
message(STATUS "BISON_VERSION=${BISON_VERSION}")
message(STATUS "parser: ${PARSER_OUTPUT};${PARSER_HEADER}")

# Force LLVM to use system installation, bypass vcpkg
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(LLVM REQUIRED CONFIG NO_CMAKE_PACKAGE_REGISTRY)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Create alias for system zstd to satisfy LLVM's dependency
if(NOT TARGET zstd::libzstd_shared)
    find_package(zstd CONFIG QUIET)
    if(NOT TARGET zstd::libzstd_shared)
        # Find system zstd library
        find_library(ZSTD_LIBRARY NAMES zstd)
        if(ZSTD_LIBRARY)
            add_library(zstd::libzstd_shared SHARED IMPORTED)
            set_target_properties(zstd::libzstd_shared PROPERTIES
                IMPORTED_LOCATION "${ZSTD_LIBRARY}")
        endif()
    endif()
endif()

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

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

add_library(minicore STATIC
  parser_bits.cpp
  parser_bits.h
  TreeNode.cpp
  TreeNode.h

  show_type_details.cpp
  llvm_helper.h

  ${FLEX_lexer_OUTPUTS}
  ${PARSER_OUTPUT}
  ${PARSER_HEADER}
  )
target_compile_options(minicore PRIVATE -Wno-register)

add_executable(compiler
  compiler.cpp
  )
target_compile_options(compiler PRIVATE -Wparentheses -Wregister -Wno-unused-result -Wno-unused-variable)
target_link_libraries(compiler minicore ${llvm_libs})

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

set (RTL_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
configure_file(mini.sh.config mini.sh @ONLY NEWLINE_STYLE UNIX)

set (TEST_MINI_DIRECTORY ${PROJECT_SOURCE_DIR}/../tests)
configure_file(all-test-compile.sh.config all-test-compile.sh @ONLY NEWLINE_STYLE UNIX)

if(BUILD_TESTS)
    enable_testing()

    # Find GoogleTest using vcpkg or system installation
    find_package(GTest REQUIRED)

    # MS VS2012 needs this to compile tuple templates
    add_definitions(-D_VARIADIC_MAX=10)
endif()

# test targets
if(BUILD_TESTS)
  add_subdirectory(test)
endif(BUILD_TESTS)


#
# install
#

install(TARGETS compiler
  RUNTIME DESTINATION bin
  )

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/mini.sh
  DESTINATION bin
  RENAME mini
  )


# end of file
