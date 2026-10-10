//
// Written by Claude on 01-Oct-26.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Harness.h"

// One test executable is one process, so this state is per executable.
static int check_count = 0;
static int failure_count = 0;
static int tests_run_count = 0;
static int tests_skipped_count = 0;
static const char* current_test = "(no test)";
static const char* name_filter = NULL; ///< NULL means "run every test".

bool harness_check(const bool passed, const char* const expression, const char* const file, const int line) {
    check_count++;
    if (!passed) {
        failure_count++;
        printf("%s:%d: in %s: CHECK(%s) failed\n", file, line, current_test, expression);
        fflush(stdout);
    }
    return passed;
}
void harness_init(const int argc, char** const argv) {
    if (argc > 2) {
        printf("usage: %s [name-filter]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 2)
        name_filter = argv[1];
}

void harness_run(void (*const test)(void), const char* const name) {
    if (name_filter != NULL && strstr(name, name_filter) == NULL) {
        tests_skipped_count++;
        return;
    }
    tests_run_count++;
    const int failures_before = failure_count;
    current_test = name;
    printf("[ RUN  ] %s\n", name);
    fflush(stdout); // if the test crashes, the buffered output (and with it the test's name) would be lost.
    test();
    printf("[ %s ] %s\n", failure_count == failures_before ? " OK " : "FAIL", name);
    current_test = "(no test)";
}
int harness_finish(void) {
    printf("%d tests run, %d checks, %d failed", tests_run_count, check_count, failure_count);
    if (name_filter != NULL)
        printf(" (filter \"%s\": %d tests skipped)", name_filter, tests_skipped_count);
    printf("\n");
    if (name_filter != NULL && tests_run_count == 0) {
        printf("no test matches the filter \"%s\"\n", name_filter);
        // a filter that runs nothing must not look like a pass.
        return EXIT_FAILURE;
    }
    return failure_count == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
