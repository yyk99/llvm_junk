#
#
#

cmake_minimum_required(VERSION 3.10)

if (POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

project (mini_compiler)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "~/.local" CACHE PATH "..." FORCE)
endif()

option(BUILD_TESTS "Build unit tests" ON)

if (BUILD_TESTS)
  enable_testing()
endif()

add_subdirectory(lib)
add_subdirectory(compiler)
