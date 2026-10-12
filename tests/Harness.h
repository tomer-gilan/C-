//
// Written by Claude on 01-Oct-26.
//

#ifndef C_COMPILER_HARNESS_H
#define C_COMPILER_HARNESS_H

#include <stdbool.h>

/**
 * Records the result of one check, and prints a report if it failed. Doesn't abort.
 * @attention use through @c CHECK , not directly.
 * @param passed the result of the check
 * @param expression the text of the checked expression
 * @param file the file the check is in
 * @param line the line the check is on
 * @return @b passed , so a test can stop early when continuing makes no sense (e.g. after a @c NULL check).
 */
bool harness_check(bool passed, const char* expression, const char* file, int line);
/**
 * Runs one test function, and prints its name only if it failed (@c [ FAIL ] after the failed checks). A passing test
 * prints nothing; if a test crashes, the sanitizer's stack trace names it.
 * If a name filter was given to @c harness_init , skips the test unless its name contains the filter.
 * @attention use through @c RUN_TEST , not directly.
 * @param test the test function
 * @param name the name of @b test
 */
void harness_run(void (*test)(void), const char* name);
/**
 * Reads the optional name filter from the command line: @c ./test_x [name-filter] runs only the tests whose name
 * contains @b name-filter , so a single test can be run without editing @c main .
 * @attention call it first in @c main , before any @c RUN_TEST . Exits with a usage message on more than one argument.
 * @param argc the argument count of @c main
 * @param argv the arguments of @c main
 */
void harness_init(int argc, char** argv);

/**
 * Prints a summary of the tests and checks so far.
 * @return @c EXIT_SUCCESS if every check passed, @c EXIT_FAILURE otherwise (also when a filter matched no test). Meant to be returned from @c main .
 */
int harness_finish(void);

/**
 * Checks that @b cond is true. On failure, prints the file, line and text of @b cond , and continues.
 * @note a function-like macro (conventions.md, Macros): only a macro can capture @c __FILE__ , @c __LINE__ and the
 * text of @b cond .
 * @return the result of the check, as a @c bool .
 */
#define CHECK(cond) harness_check((cond), #cond, __FILE__, __LINE__)
/**
 * Runs the test function @b test .
 * @note a function-like macro (conventions.md, Macros): only a macro can capture the name of @b test as text.
 */
#define RUN_TEST(test) harness_run((test), #test)

#endif //C_COMPILER_HARNESS_H
