//
// Created by tomer on 23-Sep-26.
//

#ifndef GIT_CODE_UTILS_H
#define GIT_CODE_UTILS_H

#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

typedef int bool;

/**
 * Allocated a new string, and copies the content of source to it.
 * @param source the string which is copied
 * @return the copy of the source
 */
char* utils_allocate_and_copy_string(const char* source);

/**
 *
 * @param c character
 * @return TRUE iff c is a letter
 */
bool utils_is_alpha(char c);

/**
 *
 * @param c character
 * @return TRUE iff c is a digit.
 */
bool utils_is_numeric(char c);

#endif //GIT_CODE_UTILS_H