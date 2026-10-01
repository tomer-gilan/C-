cmake_minimum_required(VERSION 4.2)
project(c_compiler C)

set(CMAKE_C_STANDARD 11)

# Lets CTest run the test executables registered below with add_test.
enable_testing()

# Warning flags, shared by every target that links this.
add_library(c_compiler_warnings INTERFACE)
target_compile_options(c_compiler_warnings INTERFACE -Wall -Wextra)

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
target_link_libraries(c_compiler_core PRIVATE c_compiler_warnings)

add_executable(c_compiler main.c)
target_link_libraries(c_compiler PRIVATE c_compiler_core c_compiler_warnings)

# -------- Tests -------- #

# The hand-rolled test harness (CHECK, RUN_TEST), shared by every test executable.
add_library(test_harness STATIC
        tests/Harness.c
        tests/Harness.h)
target_link_libraries(test_harness PRIVATE c_compiler_warnings)

add_executable(test_list tests/test_list.c)
target_link_libraries(test_list PRIVATE c_compiler_core test_harness c_compiler_warnings)
add_test(NAME list COMMAND test_list)
