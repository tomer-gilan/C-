cmake_minimum_required(VERSION 4.2)
project(git_code C)

set(CMAKE_C_STANDARD 11)

add_executable(git_code
        main.c
        Lexer.c
        Lexer.h
        Error.c
        Error.h
        DataStructures.c
        DataStructures.h
        Utils.c
        Utils.h)
