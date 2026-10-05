//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_SOURCE_TEXT_H
#define C_COMPILER_SOURCE_TEXT_H

#include <stdbool.h>

#include "Text.h"

typedef struct {
    char* name; ///< the name of the file, i.e. path==full/path/to/file_name.c and name==file_name.c
    Text* data;
} SourceText;

/**
 * Creates a new @c SourceText
 * @attention ownership of both
 * 1. @b text is transferred to the callee.
 * 2. @b name remains with the caller, i.e. name is duplicated.
 * @retval NULL if either @b text or @b name is NULL.
 */
SourceText* source_text_create(Text* text, const char* name);
/**
 * Creates a new @c SourceText from a given @b path.
 * @note adds a '\0' at the end of the copied buffer
 * @param path the path to the source file.
 * @retval NULL if @b path is invalid
 */
SourceText* source_text_create_from_path(const char* path);
/**
 * Destroys the @c SourceText .
 * @param this handle to the @c SourceText
 */
void source_text_destroy(SourceText* this);
/**
 * checks if the @b index is in the bounds of @c text.
 * @attention index == @b length is considered out-of-bounds, even though its '\0'.
 */
bool source_text_is_index_in_bounds(const SourceText* this, int index);

// for list.h
void source_text_list_destroy(void* this);

#endif //C_COMPILER_SOURCE_TEXT_H