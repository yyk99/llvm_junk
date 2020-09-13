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

BISON_TARGET(parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
  COMPILE_FLAGS "-t -v"
)
FLEX_TARGET(lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "lexer: ${FLEX_lexer_OUTPUTS}")
message(STATUS "BISON_VERSION=${BISON_VERSION}")
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

add_library(minicore STATIC
  parser_bits.cpp
  parser_bits.h
  TreeNode.cpp
  TreeNode.h

  show_type_details.cpp
  llvm_helper.h
    
  ${FLEX_lexer_OUTPUTS}
  ${BISON_parser_OUTPUTS}
  )

add_executable(compiler
  compiler.cpp
  )
target_compile_options(compiler PRIVATE -Wparentheses)
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

    include(ExternalProject)

    list(APPEND CACHE_ARGS "-DBUILD_GMOCK:BOOL=OFF")
#    list(APPEND CACHE_ARGS "-DBUILD_GTEST:BOOL=ON") # not used in 1.8.1
    list(APPEND CACHE_ARGS "-Dgtest_force_shared_crt:BOOL=ON")
    string(REPLACE ";" "$<SEMICOLON>" CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}")

    list(APPEND CACHE_ARGS "-DCMAKE_CONFIGURATION_TYPES:STRING=${CONFIGURATION_TYPES}")

    ExternalProject_Add(
        googletest-build
        GIT_REPOSITORY    https://github.com/google/googletest.git
        GIT_TAG           "release-1.8.1"
        SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
        BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
        TEST_COMMAND      ""
        CMAKE_ARGS        ${CACHE_ARGS}
        INSTALL_COMMAND   ""
#        BUILD_COMMAND     ""
    )

    set(GTEST_SOURCE_DIR ${CMAKE_BINARY_DIR}/googletest-src/googletest)
    set(GTEST_ROOT ${GTEST_SOURCE_DIR})
    set(GTEST_INCLUDE_DIR ${GTEST_ROOT}/include)
    if(WIN32)
        set(GTEST_BOTH_LIBS
            ${CMAKE_BINARY_DIR}/googletest-build/googletest/$<CONFIG>/gtest_main$<$<CONFIG:Debug>:d>.lib
            ${CMAKE_BINARY_DIR}/googletest-build/googletest/$<CONFIG>/gtest$<$<CONFIG:Debug>:d>.lib
        )
    else()
        set(GTEST_BOTH_LIBS
#            ${CMAKE_BINARY_DIR}/googletest-build/googletest/$<CONFIG>/libgtest_main.a
#            ${CMAKE_BINARY_DIR}/googletest-build/googletest/$<CONFIG>/libgtest.a
            ${CMAKE_BINARY_DIR}/googletest-build/googletest/libgtest_main.a
            ${CMAKE_BINARY_DIR}/googletest-build/googletest/libgtest.a
            -pthread
        )
    endif()      

    # MS VS2012 needs this to compile tupe templates
    add_definitions(-D_VARIADIC_MAX=10)
    include_directories(${GTEST_INCLUDE_DIR} ${GTEST_ROOT})
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
