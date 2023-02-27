cmake_minimum_required(VERSION 3.24)
project(Vault C)

set(CMAKE_C_STANDARD 99)
set(INCLUDE_DIRECTORY src/include)
set(SOURCES
        src/c/main.c
        src/include/vault.h
        src/include/types.h
        src/include/util.h
        src/include/io.h
        src/c/vlt.c
        src/c/io.c
        src/c/util.c src/include/assert.h)

add_executable(Vault ${SOURCES})

target_include_directories(Vault PRIVATE ${INCLUDE_DIRECTORY})

add_compile_definitions(_WINDOWS)
add_compile_definitions(_LINUX)
