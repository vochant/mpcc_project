cmake_minimum_required(VERSION 3.5)
project(mpcc LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXEC_NAME mpcc)
set(EXEC_DESC "MPCC - A MPC Programming Language Toolchain")
set(VERSION 1.0)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/EHsc>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:utf-8>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

set(RES_DIR res)

# find_package(nlohmann_json REQUIRED)

# src
set(LIBRARY_SOURCES
    src/ast/ast.cpp
    src/lexer/token.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/util.cpp
    src/convertor/asm2plain.cpp
    src/asm/asm.cpp
)

set(PROGRAM_SOURCES src/main.cpp)

add_library(mpcclib OBJECT ${LIBRARY_SOURCES})
target_include_directories(mpcclib PUBLIC
    include
    ${PROJECT_BINARY_DIR}
)
#target_link_libraries(${MPCC_LIB} PUBLIC
#    nlohmann_json::nlohmann_json
#)

add_executable(mpcc ${PROGRAM_SOURCES})
target_link_libraries(mpcc PRIVATE mpcclib)

# add_custom_command(
#     TARGET ${MPCC_PROG}
#     POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/${RES_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${RES_DIR}
# )