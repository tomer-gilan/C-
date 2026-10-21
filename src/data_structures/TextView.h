//
// Created by tomer on 05-Oct-26.
//

#ifndef C_COMPILER_TEXTVIEW_H
#define C_COMPILER_TEXTVIEW_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
    int length; ///< length of @b data.
    const char* data; ///< Must not be @c NULL
} TextView;

/**
 * This macro generates an INITIALIZER based on a string literal @b literal .
 * @note as a general guideline, no macros of this style are welcome in this project.
 * However, a compile-time-calculated expression was needed, and this solution far too simple and elegant to ignore.
 * @param literal
 * @return an INITIALIZER of a @c TextView
 */
#define TEXT_VIEW_INITIALIZER_OF_LITERAL(literal) {.data = "" literal "", .length = sizeof(literal) - 1}
/**
 * This macro generates an initialization list for a @c TextView, sourced specifically from a string literal.
 * @warning passing anything other than string literals will cause a compilation error.
 * @note as a general guideline, no macros of this style are welcome in this project.
 * However, a compile-time-calculated expression was needed, and this solution far too simple and elegant to ignore.
 * @param literal a string literal, to be taken as the source for the @c TextView
 * @return a @c TextView
 */
#define text_view_value_of_literal(literal) ((TextView)TEXT_VIEW_INITIALIZER_OF_LITERAL(literal))
/**
 * @attention @b data must outlive the return value, since the returned @c TextView points to @b data.
 */
TextView text_view_make(int length, const char* data);
bool text_view_is_empty(TextView this);
/**
 * predicate, checks if both are equal
 */
bool text_view_equals(TextView first, TextView second);
void text_view_print(TextView this, FILE* output);
// for list.h usage
bool text_view_list_equals(const void* first, const void* second);
/**
 * @attention this is shallow duplication
 */
void* text_view_list_duplicate(const void* text_view);

#endif //C_COMPILER_TEXTVIEW_H