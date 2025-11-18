#
#
#

cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

project (mini_compiler)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "~/.local" CACHE PATH "..." FORCE)
endif()

option(BUILD_TESTS "Build unit tests" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (BUILD_TESTS)
  enable_testing()
endif()

add_subdirectory(lib)
add_subdirectory(compiler)
