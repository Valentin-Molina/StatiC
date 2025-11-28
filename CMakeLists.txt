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
)

add_executable(static ${SOURCES})