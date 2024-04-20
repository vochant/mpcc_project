cmake_minimum_required(VERSION 3.5)
project(lighzy-interpreter LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COLORED_OUTPUTS true)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)

configure_file(config.h.in config.h)

option(BUILD_TEST "build the target test" ON)

set(CMAKE_CXX_FLAGS "-ftemplate-backtrace-limit=1")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug")
endif()

# src
set(LIBRARY_SOURCES
    src/lexer/token.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/object/base.cpp
    src/evaluator/evaluator.cpp
    src/program/program.cpp
)

set(PROGRAM_SOURCES
    src/main.cpp)

add_library(mpcc_lib OBJECT ${LIBRARY_SOURCES})
target_include_directories(mpcc_lib PUBLIC
    include
    ${PROJECT_BINARY_DIR}
)
target_link_libraries(mpcc_lib PUBLIC)

add_executable(mpcc ${PROGRAM_SOURCES})
target_link_libraries(mpcc PRIVATE
    mpcc_lib
)