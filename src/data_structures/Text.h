//
// Created by tomer on 05-Oct-26.
//

#ifndef C_COMPILER_TEXT_H
#define C_COMPILER_TEXT_H

#include <stdbool.h>

#include "TextView.h"

typedef struct {
    int length;
    char* data;
    int capacity;
} Text;

Text* text_create_empty(void);
Text* text_create_from_view(TextView text_view);
/**
 * @param buffer caller owned buffer, to be duplicated.
 * @param length length of @b buffer
 * @return handle to @c Text
 */
Text* text_create_from_buffer(const char* buffer, int length);
/**
 * @retval NULL if path is invalid
 */
Text* text_create_from_path(const char* path);
/**
 * destroys the @c Text struct.
 */
void text_destroy(Text* this);
/**
 * predicate, checks if both are equal
 */
bool text_is_empty(const Text* this);
bool text_equals(const Text* first, const Text* second);
/**
 * pushes @c c into @c data, possibly extending its capacity.
 */
void text_push(Text* this, char c);
/**
 * @attention @b this must outlive the return value, since it points to @b this
 * returns a @c TextView of @b this
 */
TextView text_as_view(const Text* this);

// for list.h usage
bool text_list_equals(const void* first, const void* second);

/**
 * @attention this is DEEP duplication
 */
void* text_list_duplicate(const void* text);
void text_list_destroy(void* this);

#endif //C_COMPILER_TEXT_H