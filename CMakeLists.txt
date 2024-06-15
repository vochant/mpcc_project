cmake_minimum_required(VERSION 3.5)
project(lighzy-interpreter LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXEC_NAME mpcc)
set(EXEC_DESC "MPCC - A MPC Programming Language Toolchain")
set(VERSION 1.0)

set(COLORED_OUTPUTS true)
set(NO_MODULE false)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

set(RES_DIR res)

configure_file(config.h.in config.h)

set(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=1")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug")
endif()

set(MPCC_PROG mpcc)
set(MPCC_LIB mpcc_lib)

find_package(nlohmann_json REQUIRED)

# src
set(LIBRARY_SOURCES
    src/ast/base/node.cpp
    src/lexer/token.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/object/base.cpp
    src/object/environment.cpp
    src/object/instance.cpp
    src/evaluator/evaluator.cpp
    src/program/program.cpp
)

set(PROGRAM_SOURCES
    src/main.cpp)

add_library(mpcc_lib OBJECT ${LIBRARY_SOURCES})
target_include_directories(${MPCC_PROG} PUBLIC
    include
    ${PROJECT_BINARY_DIR}
)
target_link_libraries(${MPCC_LIB} PUBLIC
    nlohmann_json::nlohmann_json
)

add_executable(${MPCC_PROG} ${PROGRAM_SOURCES})
target_link_libraries(${MPCC_PROG} PRIVATE
    ${MPCC_LIB}
)

add_custom_command(
    TARGET ${MPCC_PROG}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/${RES_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${RES_DIR}
)