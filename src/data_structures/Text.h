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
 * Writes the contents of @b this to the file at @b path , creating it or replacing what it held.
 * @note ownership of @b this remains with the caller.
 * @retval false if the file can't be opened or fully written. A partly written file is removed.
 */
bool text_write_to_path(const Text* this, const char* path);
Text* text_create_from_int(int number);
/**
 * destroys the @c Text struct.
 */
void text_destroy(Text* this);
bool text_is_empty(const Text* this);
bool text_equals(const Text* first, const Text* second);
/**
 * @attention @b this must outlive the return value, since it points to @b this
 * returns a @c TextView of @b this
 */
TextView text_as_view(const Text* this);

/**
 * pushes @c c into @c data, possibly extending its capacity.
 */
void text_append_char(Text* this, char c);
void text_append_text_view(Text* this, TextView view);
/**
 * @note @b this never references @b other as a result of this function call
 */
void text_append_text(Text* this, const Text* other);
void text_append_int(Text* this, int number);

// for list.h usage
bool text_list_equals(const void* first, const void* second);
/**
 * @attention this is DEEP duplication
 */
void* text_list_duplicate(const void* text);
void text_list_destroy(void* this);

#endif //C_COMPILER_TEXT_H