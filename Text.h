//
// Created by tomer on 05-Oct-26.
//

#ifndef C_COMPILER_TEXT_H
#define C_COMPILER_TEXT_H
#include <stdbool.h>

typedef struct {
    int length;
    char* data;
} Text;

/**
 * @param buffer callee owned buffer, to be duplicated.
 * @param length length of @b buffer
 * @return handle to @c String
 */
Text* text_create_from_buffer(const char* buffer, int length);
Text* text_create_from_file(const char* path);
/**
 * destroys the @c String struct.
 */
void text_destroy(Text* this);
/**
 * predicate, checks if both are equal
 */
bool text_equals(const Text* first, const Text* second);
int text_compare(const Text* first, const Text* second);

// for list.h usage
int text_list_equals(const void* first, const void* second);
int text_list_compare(const void* first, const void* second);
/**
 * @attention this is DEEP duplication
 */
void* text_list_duplicate(const void* text);

#endif //C_COMPILER_TEXT_H