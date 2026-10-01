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
 * Runs one test function, and prints its name and whether it passed.
 * @attention use through @c RUN_TEST , not directly.
 * @param test the test function
 * @param name the name of @b test
 */
void harness_run(void (*test)(void), const char* name);
/**
 * Prints a summary of every check so far.
 * @return @c EXIT_SUCCESS if every check passed, @c EXIT_FAILURE otherwise. Meant to be returned from @c main .
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
