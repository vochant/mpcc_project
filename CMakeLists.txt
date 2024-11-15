cmake_minimum_required(VERSION 3.5)
project(mpcc LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=default")
endif()

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
    src/env/environment.cpp
    src/lexer/token.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/util.cpp
    src/object/instance.cpp
    src/object/object.cpp
    src/program/program.cpp
    src/system/system.cpp
    src/vm/vm.cpp
)

set(PROGRAM_SOURCES src/main.cpp)

add_library(mpcclib SHARED ${LIBRARY_SOURCES})
set_target_properties(mpcclib PROPERTIES OUTPUT_NAME "mpcc")
target_include_directories(mpcclib PUBLIC
    include
    ${PROJECT_BINARY_DIR}
)
#target_link_libraries(${MPCC_LIB} PUBLIC
#    nlohmann_json::nlohmann_json
#)

add_executable(mpcc ${PROGRAM_SOURCES})
target_link_libraries(mpcc PRIVATE mpcclib)