//
// Created by tomer on 05-Oct-26.
//

#ifndef C_COMPILER_TEXTVIEW_H
#define C_COMPILER_TEXTVIEW_H

#include <stdbool.h>

typedef struct {
    int length; ///< length of @b data.
    const char* data; ///< Must not be @c NULL
} TextView;

/**
 * This macro generates an initialization list for a @c StringView, sourced specifically from a string literal.
 * @warning passing anything other than string literals will cause a compilation error.
 * @note as a general guideline, no macros of this style are welcome in this project.
 * However, a compile-time-calculated expression was needed, and this solution far too simple and elegant to ignore.
 * @param literal a string literal, to be taken as the source for the @c StringView
 * @return a @c StringView
 */
#define text_view_from_literal(literal) ((TextView){.data = "" literal "", .length = sizeof(literal) - 1})

/**
 * predicate, checks if both are equal
 */
bool text_view_equals(TextView first, TextView second);

// for list.h usage
int text_view_list_compare(const void* first, const void* second);
/**
 * @attention this is shallow duplication
 */
void* text_view_list_duplicate(const void* string_view);

#endif //C_COMPILER_TEXTVIEW_H