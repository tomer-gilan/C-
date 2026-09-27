//
// Created by tomer on 23-Sep-26.
//

#ifndef GIT_CODE_UTILS_H
#define GIT_CODE_UTILS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char byte;

// -------------------- string utils -------------------- //
/**
 * Allocated a new string, and copies the content of source to it.
 * @param source the string which is copied
 * @return the copy of the source
 */
char* utils_allocate_and_copy_string(const char* source);

/**
 *
 * @param string the element to find the index of
 * @param arr the array to search in
 * @param arr_length the length of 'arr'
 * @return the index of 'string' in 'arr' (-1 if not found).
 */
int utils_string_array_index_of(const char* string, const char* const* arr, unsigned int arr_length);

/**
 * Functionally the exact same as strcmp, just taking void* instead of char*
 * @param first the first string
 * @param second the second string
 * @return the same as strcmp
 */
int list_string_strcmp(const void* first, const void* second);

#endif //GIT_CODE_UTILS_H