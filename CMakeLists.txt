cmake_minimum_required(VERSION 4.2)
project(git_code C)

set(CMAKE_C_STANDARD 11)

add_executable(git_code
        main.c
        Lexer.c
        Lexer.h
        Diagnostic.c
        Diagnostic.h
        DataStructures.c
        DataStructures.h
        Utils.c
        Utils.h
        Token.c
        Token.h
        Globals.c
        Globals.h)
