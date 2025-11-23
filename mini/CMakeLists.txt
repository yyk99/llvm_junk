#
#
#

cmake_minimum_required(VERSION 3.20)

if (POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

option(BUILD_TESTS "Build unit tests" ON)
if (BUILD_TESTS)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()

option(BUILD_LLVM "Build llvm from scratch" OFF)
if (BUILD_LLVM)
    list(APPEND VCPKG_MANIFEST_FEATURES "llvm-from-scratch")
endif()

project (mini_compiler)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  if(WIN32)
    set(USER_HOME_DIRECTORY "$ENV{USERPROFILE}")
  else()
    set(USER_HOME_DIRECTORY "$ENV{HOME}")
  endif()
  set(CMAKE_INSTALL_PREFIX "${USER_HOME_DIRECTORY}/.local" CACHE PATH "..." FORCE)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (BUILD_TESTS)
  enable_testing()
endif()

add_subdirectory(lib)
add_subdirectory(compiler)
