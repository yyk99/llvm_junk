cmake_minimum_required(VERSION 3.10)
project(examples)

if(POLICY CMP0068)
  cmake_policy(SET CMP0068 NEW)
  set(CMAKE_BUILD_WITH_INSTALL_NAME_DIR ON)
endif()

if (POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

#option(LLVM_ENABLE_EH "Build LLVM with exception-handling support." ON) 
set(LLVM_ENABLE_EH OFF)

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

function (add_llvm_example toy)
  # Now build our tools
  if ( ${ARGC} EQUAL 1 )
     add_executable(${toy} ${toy}.cpp)
  else ()
     add_executable(${toy} ${ARGN})
  endif()

  # Find the libraries that correspond to the LLVM components
  # that we wish to use
  llvm_map_components_to_libnames(
	llvm_libs
	core
	mcjit
	native
	orcjit
  )

  # Link against LLVM libraries
  target_link_libraries(${toy} ${llvm_libs})
endfunction (add_llvm_example)

function(export_executable_symbols target)
  if (LLVM_EXPORTED_SYMBOL_FILE)
    # The symbol file should contain the symbols we want the executable to
    # export
    set_target_properties(${target} PROPERTIES ENABLE_EXPORTS 1)
  elseif (LLVM_EXPORT_SYMBOLS_FOR_PLUGINS)
    # Extract the symbols to export from the static libraries that the
    # executable links against.
    set_target_properties(${target} PROPERTIES ENABLE_EXPORTS 1)
    set(exported_symbol_file ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${target}.symbols)
    # We need to consider not just the direct link dependencies, but also the
    # transitive link dependencies. Do this by starting with the set of direct
    # dependencies, then the dependencies of those dependencies, and so on.
    get_target_property(new_libs ${target} LINK_LIBRARIES)
    set(link_libs ${new_libs})
    while(NOT "${new_libs}" STREQUAL "")
      foreach(lib ${new_libs})
        if(TARGET ${lib})
          get_target_property(lib_type ${lib} TYPE)
          if("${lib_type}" STREQUAL "STATIC_LIBRARY")
            list(APPEND static_libs ${lib})
          else()
            list(APPEND other_libs ${lib})
          endif()
          get_target_property(transitive_libs ${lib} INTERFACE_LINK_LIBRARIES)
          foreach(transitive_lib ${transitive_libs})
            if(TARGET ${transitive_lib} AND NOT ${transitive_lib} IN_LIST link_libs)
              list(APPEND newer_libs ${transitive_lib})
              list(APPEND link_libs ${transitive_lib})
            endif()
          endforeach(transitive_lib)
        endif()
      endforeach(lib)
      set(new_libs ${newer_libs})
      set(newer_libs "")
    endwhile()
    list(REMOVE_DUPLICATES static_libs)
    if (MSVC)
      set(mangling microsoft)
    else()
      set(mangling itanium)
    endif()
    get_host_tool_path(llvm-nm LLVM_NM llvm_nm_exe llvm_nm_target)
    get_host_tool_path(llvm-readobj LLVM_READOBJ llvm_readobj_exe llvm_readobj_target)
    add_custom_command(OUTPUT ${exported_symbol_file}
                       COMMAND "${Python3_EXECUTABLE}"
                         ${LLVM_MAIN_SRC_DIR}/utils/extract_symbols.py
                         --mangling=${mangling} ${static_libs}
                         -o ${exported_symbol_file}
                         --nm=${llvm_nm_exe}
                         --readobj=${llvm_readobj_exe}
                       WORKING_DIRECTORY ${LLVM_LIBRARY_OUTPUT_INTDIR}
                       DEPENDS ${LLVM_MAIN_SRC_DIR}/utils/extract_symbols.py
                         ${static_libs} ${llvm_nm_target} ${llvm_readobj_target}
                       VERBATIM
                       COMMENT "Generating export list for ${target}")
    add_llvm_symbol_exports( ${target} ${exported_symbol_file} )
    # If something links against this executable then we want a
    # transitive link against only the libraries whose symbols
    # we aren't exporting.
    set_target_properties(${target} PROPERTIES INTERFACE_LINK_LIBRARIES "${other_libs}")
    # The default import library suffix that cmake uses for cygwin/mingw is
    # ".dll.a", but for clang.exe that causes a collision with libclang.dll,
    # where the import libraries of both get named libclang.dll.a. Use a suffix
    # of ".exe.a" to avoid this.
    if(CYGWIN OR MINGW)
      set_target_properties(${target} PROPERTIES IMPORT_SUFFIX ".exe.a")
    endif()
  elseif(NOT (WIN32 OR CYGWIN))
    # On Windows auto-exporting everything doesn't work because of the limit on
    # the size of the exported symbol table, but on other platforms we can do
    # it without any trouble.
    set_target_properties(${target} PROPERTIES ENABLE_EXPORTS 1)
    # CMake doesn't set CMAKE_EXE_EXPORTS_${lang}_FLAG on Solaris, so
    # ENABLE_EXPORTS has no effect.  While Solaris ld defaults to -rdynamic
    # behaviour, GNU ld needs it.
    if (APPLE OR ${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
      set_property(TARGET ${target} APPEND_STRING PROPERTY
        LINK_FLAGS " -rdynamic")
    endif()
  endif()
endfunction()

add_subdirectory(BrainF)
add_subdirectory(Fibonacci)
add_subdirectory(HowToUseJIT)
add_subdirectory(Kaleidoscope)
add_subdirectory(ModuleMaker)

if(LLVM_ENABLE_EH AND (NOT WIN32) AND (NOT "${LLVM_NATIVE_ARCH}" STREQUAL "ARM"))
    add_subdirectory(ExceptionDemo)
endif()

if( HAVE_PTHREAD_H )
    add_subdirectory(ParallelJIT)
endif( HAVE_PTHREAD_H )
