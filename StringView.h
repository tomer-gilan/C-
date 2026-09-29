//
// Created by tomer on 28-Sep-26.
//

#ifndef C_COMPILER_STRINGVIEW_H
#define C_COMPILER_STRINGVIEW_H

#include <string.h>
#include <stdbool.h>

typedef struct {
    const char* data;
    int length;
} StringView;

/**
 * @warning assumes @b string outlives @c StringView
 * @param string the @b '\0' terminated source buffer.
 */
StringView string_view_from_string(const char* string);
/**
 * This macro generates an initialization list for a @c StringView, sourced specifically from a string literal.
 * @warning passing anything other than string literals will cause a compilation error.
 * @note as a general guideline, no macros of this style are welcome in this project.
 * However, a compile-time-calculated expression was needed, and this solution far too simple and elegant to ignore.
 * @param literal a string literal, to be taken as the source for the @c StringView
 * @return a @c StringView
 */
#define string_view_from_literal(literal) ((StringView){.data = "" literal "", .length = sizeof(literal) - 1})
bool string_view_equals(StringView first, StringView second);

// for list.h usage
int string_view_list_equals(const void* first, const void* second);
/**
 * @attention this is shallow duplication
 */
void* string_view_list_duplicate(const void* string_view);

#endif //C_COMPILER_STRINGVIEW_H