//
// Created by tomer on 23-Sep-26.
//

#ifndef C_COMPILER_UTILS_H
#define C_COMPILER_UTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// -------------------- string utils -------------------- //
/**
 * Allocated a new string, and copies the content of source to it.
 * @param source the string which is copied
 * @return the copy of the source
 */
char* utils_allocate_and_copy_string(const char* source);

/**
 * Functionally the exact same as strcmp, just taking void* instead of char*
 * @param first the first string
 * @param second the second string
 * @return the same as strcmp
 */
int utils_string_list_strcmp(const void* first, const void* second);
// for list.h
void* utils_string_list_allocate_and_copy(const void* string);

#endif //C_COMPILER_UTILS_H