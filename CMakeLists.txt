cmake_minimum_required(VERSION 3.24)
project(Pwm C)

set(CMAKE_C_STANDARD 99)
set(INCLUDE_DIRECTORY src/include)
set(SOURCES
        src/c/main.c
        src/include/vault.h
        src/c/vault.c
        src/include/io.h
        src/c/io.c
        src/include/types.h
        src/c/cmds/init.c
        src/include/cmds/init.h
        src/include/cmds/add.h
        src/include/cmds/cp.h
        src/c/cmds/add.c src/c/cmds/cp.c
        test/iotest.c src/include/util.h src/c/vlt.c)

add_executable(Pwm ${SOURCES})

target_include_directories(Pwm PRIVATE ${INCLUDE_DIRECTORY})

add_compile_definitions(_WINDOWS)
add_compile_definitions(_LINUX)
