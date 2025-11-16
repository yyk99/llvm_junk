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
        message(STATUS "add_executable(${dir}_${prog} ${src})")

        add_executable(${dir}_${prog} ${src})
        gtest_discover_tests(${dir}_${prog})
        # add_test(${dir}_${prog} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${dir}_${prog})
        set_target_properties(${dir}_${prog} PROPERTIES COMPILE_DEFINITIONS "TESTING=1")
        target_link_libraries(${dir}_${prog} GTest::gtest GTest::gtest_main)
        target_link_libraries(${dir}_${prog} ${llvm_libs})
        target_link_libraries(${dir}_${prog} minicore)
    endforeach()
endmacro(make_test_executables)

include(GoogleTest)
make_test_executables()

#add_executable(test_t1 t1.cpp)
#add_test(test_t1 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_t1)
#set_target_properties(test_t1 PROPERTIES COMPILE_DEFINITIONS "TESTING=1")
#target_link_libraries(test_t1 ${GTEST_BOTH_LIBS})
#add_dependencies(test_t1 googletest-build)

