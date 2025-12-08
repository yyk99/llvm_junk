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

option(BUILD_EXAMPLES "Build example (flex and bison) apps" OFF)
if (BUILD_EXAMPLES)
    # list(APPEND VCPKG_MANIFEST_FEATURES "build-example-apps")
endif()

option(USE_LLVM_COMPONENT_SUBSET "Use a subset of compoments to save build time" OFF)

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
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set(AUX_TARGETS
    "acc_gen"
    "omp_gen"
    "RISCVTargetParserTableGen"
    "intrinsics_gen"
)
foreach(t IN ITEMS ${AUX_TARGETS})
    if (TARGET ${t})
        set_target_properties(${t} PROPERTIES FOLDER "Aux targets")
    endif()
endforeach()
