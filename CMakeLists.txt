cmake_minimum_required(VERSION 4.2)
project(c_compiler C)

set(CMAKE_C_STANDARD 11)

# Everything except main.c, so test executables can link it with their own main.
add_library(c_compiler_core STATIC
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
        FilePosition.c
        FilePosition.h
        SourceText.c
        SourceText.h
        Context.c
        Context.h
        Driver.c
        Driver.h
        Range.c
        Range.h
        StringView.c
        StringView.h)
target_include_directories(c_compiler_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(c_compiler main.c)
target_link_libraries(c_compiler PRIVATE c_compiler_core)
