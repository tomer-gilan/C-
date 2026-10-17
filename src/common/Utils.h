//
// Created by tomer on 23-Sep-26.
//

#ifndef C_COMPILER_UTILS_H
#define C_COMPILER_UTILS_H

#include <stdbool.h>
#include <stddef.h>

// -------------------- memory allocation -------------------- //
/**
 * Wrapper of @c malloc.
 * Only difference is that upon allocation failure, reports to @c stderr and @c abort
 * @note @c size_t is deliberately used to align with @c malloc
 * @retval NULL if @b size == 0, may be returned.
 */
void* utils_xmalloc(size_t size);
/**
 * Wrapper of @c realloc.
 * Only difference is that upon allocation failure, reports to @c stderr and @c abort
 * @note @c size_t is deliberately used to align with @c realloc
 * @retval NULL if @b size == 0, may be returned.
 */
void* utils_xrealloc(void* ptr, size_t size);
/**
 * Wrapper of @c calloc.
 * Only difference is that upon allocation failure, reports to @c stderr and @c abort
 * @note @c size_t is deliberately used to align with @c malloc
 * @retval NULL if @b size == 0, may be returned.
 */
void* utils_xcalloc(size_t element_sizeof, int element_count);

// --------- abortion utils ------------ //
/**
 * prints message to @c stderr, then @c abort
 * @param string the message string to print. Isn't required a '\n' at the end (i.e. one will be added)
 */
void utils_abort_with_message(const char* string);

// -------------------- string utils -------------------- //
/**
 * Allocated a new string, and copies the content of source to it.
 * @warning ownership of the returned string transfers to the caller.
 * @param source the string which is copied
 * @return the copy of the source
 */
char* utils_allocate_and_copy_string(const char* source);

bool utils_string_list_equals(const void* first, const void* second);

// for list.h
void* utils_string_list_allocate_and_copy(const void* string);

// -------------------- other -------------------- //
/**
 * returns a newly allocated copy of @b buffer
 * @warning ownership of the returned buffer transfers to the caller.
 */
char* utils_allocate_and_copy_buffer(const char* buffer, int size);

#endif //C_COMPILER_UTILS_H