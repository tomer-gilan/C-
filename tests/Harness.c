//
// Written by Claude on 01-Oct-26.
//

#include <stdio.h>
#include <stdlib.h>

#include "Harness.h"

// One test executable is one process, so this state is per executable.
static int check_count = 0;
static int failure_count = 0;
static const char* current_test = "(no test)";

bool harness_check(const bool passed, const char* const expression, const char* const file, const int line) {
    check_count++;
    if (!passed) {
        failure_count++;
        printf("%s:%d: in %s: CHECK(%s) failed\n", file, line, current_test, expression);
        fflush(stdout);
    }
    return passed;
}
void harness_run(void (*const test)(void), const char* const name) {
    const int failures_before = failure_count;
    current_test = name;
    printf("[ RUN  ] %s\n", name);
    fflush(stdout); // if the test crashes, the buffered output (and with it the test's name) would be lost.
    test();
    printf("[ %s ] %s\n", failure_count == failures_before ? " OK " : "FAIL", name);
    current_test = "(no test)";
}
int harness_finish(void) {
    printf("%d checks, %d failed\n", check_count, failure_count);
    return failure_count == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
