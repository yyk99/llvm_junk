#
#
#

cmake_minimum_required(VERSION 3.20)
project(examples_flex)

find_package(FLEX)

if (NOT FLEX_INCLUDE_DIR)
    if (FLEX_EXECUTABLE)
        # Get the Cygwin root directory from the flex path
        get_filename_component(CYGWIN_BIN_DIR ${FLEX_EXECUTABLE} DIRECTORY)
        get_filename_component(CYGWIN_ROOT ${CYGWIN_BIN_DIR} DIRECTORY)
    
        # Look for FlexLexer.h in Cygwin's include directory
        find_path(FLEX_INCLUDE_DIR
            NAMES FlexLexer.h
            PATHS
                "${CYGWIN_ROOT}/usr/include"
                "${CYGWIN_ROOT}/include"
            NO_DEFAULT_PATH
        )
        message(STATUS "FLEX_INCLUDE_DIR=${FLEX_INCLUDE_DIR}")
    endif()
endif()

FLEX_TARGET(lexer lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
    COMPILE_FLAGS "-+ --nounistd"
)

message(STATUS "FLEX_lexer_OUTPUTS=${FLEX_lexer_OUTPUTS}")

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${FLEX_INCLUDE_DIR})

add_executable(scanner
    scanner.cpp
    scanner.h
    lexer.l
    ${FLEX_lexer_OUTPUTS}
)

if (MSVC)
    target_compile_options(scanner PRIVATE /wd4244 /wd4267 /wd4624)
    target_compile_definitions(scanner PRIVATE -D__STDC_VERSION__=199901)
else()
    target_compile_options(scanner PRIVATE -g)
endif()

message(STATUS "FLEX_LIBRARIES=${FLEX_LIBRARIES}")
if (MSVC)
    set(FLEX_LIBRARIES)
else()
    set(FLEX_LIBRARIES "-lfl")
endif()
target_link_libraries(scanner ${FLEX_LIBRARIES} )

set_target_properties(scanner PROPERTIES FOLDER "Example Apps")
