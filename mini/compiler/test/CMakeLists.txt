#
#
#

macro(make_test_executables)
  file(GLOB source_files "*.cpp")
  foreach(cpp ${source_files})
    get_filename_component(dir ${cpp} DIRECTORY) 
    get_filename_component(dir ${dir} NAME) 
    get_filename_component(prog ${cpp} NAME_WE) 
    get_filename_component(src ${cpp} NAME)

    set (test_name "${prog}")
    message(STATUS "add_executable(${test_name} ${src})")

    add_executable(${test_name} ${src})
    add_test(${test_name} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${test_name})
    set_target_properties(${test_name} PROPERTIES COMPILE_DEFINITIONS "TESTING=1")
    target_link_libraries(${test_name} ${GTEST_BOTH_LIBS})
    target_link_libraries(${test_name} ${llvm_libs})
    target_link_libraries(${test_name} minicore)
    add_dependencies(${test_name} googletest-build)
  endforeach()
endmacro(make_test_executables)

make_test_executables()

#add_executable(test_t1 t1.cpp)
#add_test(test_t1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_t1)
#set_target_properties(test_t1 PROPERTIES COMPILE_DEFINITIONS "TESTING=1")
#target_link_libraries(test_t1 ${GTEST_BOTH_LIBS})
#add_dependencies(test_t1 googletest-build)

