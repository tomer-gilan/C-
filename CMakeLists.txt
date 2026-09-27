cmake_minimum_required(VERSION 4.2)
project(c_compiler C)

set(CMAKE_C_STANDARD 11)

add_executable(c_compiler
        main.c
        Lexer.c
        Lexer.h
        Diagnostic.c
        Diagnostic.h
        List.c
        List.h
        Utils.c
        Utils.h
        Token.c
        Token.h
        Context.c
        Context.h
        Location.c
        Location.h)
