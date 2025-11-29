cmake_minimum_required(VERSION 3.19)

project(StatiC C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

set(SOURCES
    src/main.c
    src/server.c
    src/connection.c
    src/parser.c
)

add_executable(static ${SOURCES})

option(BUILD_TESTING "Build static unit test")
if(BUILD_TESTING) 
    enable_testing()    
    add_executable(test_parser test/test_parser.c src/parser.c)
    add_test(NAME test_parser
         COMMAND test_parser)
endif()