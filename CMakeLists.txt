cmake_minimum_required(VERSION 4.2)
project(c_compiler C)

set(CMAKE_C_STANDARD 11)

# Lets CTest run the test executables registered below with add_test.
enable_testing()

# Warning flags, shared by every target that links this.
add_library(c_compiler_warnings INTERFACE)
target_compile_options(c_compiler_warnings INTERFACE -Wall -Wextra)

# Instrumented builds (testing-roadmap.md, steps 6 and 9). The sanitizers are on by default in Debug builds, so every
# test run checks memory and undefined behavior; -DENABLE_SANITIZERS=OFF turns them off (Release never has them unless
# asked). Coverage is off by default.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(sanitizers_default ON)
else ()
    set(sanitizers_default OFF)
endif ()
option(ENABLE_SANITIZERS "Build with AddressSanitizer, LeakSanitizer and UndefinedBehaviorSanitizer" ${sanitizers_default})
option(ENABLE_COVERAGE "Build unoptimized with gcov coverage counters (see tests/coverage.sh)" OFF)

# Compile and link flags for the options above, shared like the warnings. Linked PUBLIC by the library so that every
# executable linking it gets the link flags too.
add_library(c_compiler_instrumentation INTERFACE)
if (ENABLE_SANITIZERS)
    # no-sanitize-recover: a UBSan finding stops the program (and fails the test) instead of printing and carrying on.
    target_compile_options(c_compiler_instrumentation INTERFACE
            -fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer)
    target_link_options(c_compiler_instrumentation INTERFACE -fsanitize=address,undefined)
endif ()
if (ENABLE_COVERAGE)
    # -O0: optimization merges and removes lines and branches, which makes the counts hard to read.
    target_compile_options(c_compiler_instrumentation INTERFACE --coverage -O0 -g)
    target_link_options(c_compiler_instrumentation INTERFACE --coverage)
endif ()

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
        Text.c
        Text.h
        TextView.c
        TextView.h)
# -iquote (not -I, which target_include_directories would give): the project's headers are found by #include "x.h"
# only, so a header whose name matches a system header (the case-insensitive /mnt/c: String.h vs <string.h>) can
# never hide the system one from #include <x.h>. Propagates to every target that links the library.
target_compile_options(c_compiler_core PUBLIC "SHELL:-iquote ${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(c_compiler_core PRIVATE c_compiler_warnings PUBLIC c_compiler_instrumentation)

add_executable(c_compiler main.c)
target_link_libraries(c_compiler PRIVATE c_compiler_core c_compiler_warnings)

# -------- Tests -------- #

# The hand-rolled test harness (CHECK, RUN_TEST), shared by every test executable.
add_library(test_harness STATIC
        tests/Harness.c
        tests/Harness.h)
target_link_libraries(test_harness PRIVATE c_compiler_warnings c_compiler_instrumentation)

add_executable(test_list tests/test_list.c)
target_link_libraries(test_list PRIVATE c_compiler_core test_harness c_compiler_warnings)
add_test(NAME list COMMAND test_list)

add_executable(test_text_view tests/test_text_view.c)
target_link_libraries(test_text_view PRIVATE c_compiler_core test_harness c_compiler_warnings)
add_test(NAME text_view COMMAND test_text_view)

add_executable(test_lexer tests/test_lexer.c)
target_link_libraries(test_lexer PRIVATE c_compiler_core test_harness c_compiler_warnings)
add_test(NAME lexer COMMAND test_lexer)

add_executable(test_source_text tests/test_source_text.c)
target_link_libraries(test_source_text PRIVATE c_compiler_core test_harness c_compiler_warnings)
add_test(NAME source_text COMMAND test_source_text)

if (ENABLE_SANITIZERS)
    # ASan and LeakSanitizer already print a stack trace; UBSan only does on request.
    set_tests_properties(list text_view lexer source_text PROPERTIES ENVIRONMENT "UBSAN_OPTIONS=print_stacktrace=1")
endif ()
