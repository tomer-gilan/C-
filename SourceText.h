//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_SOURCE_TEXT_H
#define C_COMPILER_SOURCE_TEXT_H

#include <stdbool.h>

typedef struct {
    char* name; ///< the name of the file, i.e. path==full/path/to/file_name.c and name==file_name.c
    char* text;
    int length;
} SourceText;

/**
 * Creates a new @c SourceText from a given @b path.
 * @note adds a '\0' at the end of the copied buffer
 * @param path the path to the source file.
 * @retval NULL if @b path is invalid
 */
SourceText* source_text_create_from_path(const char* path);
/**
 * Creates a new @c SourceText from a given @b string.
 * @note adds a '\0' at the end of the copied buffer
 * @attention ownership of @b string REMAINS at the callee, i.e. content of @b string is duplicated.
 * @param buffer the source data (may include @c '\0')
 * @param length the length of the @b buffer
 * @retval NULL if @b length is 0
 */
SourceText* source_text_create_from_buffer(const char* buffer, int length);
/**
 * Destroys the @c SourceText .
 * @param this handle to the @c SourceText
 */
void source_text_destroy(SourceText* this);
/**
 * checks if the @b index is in the bounds of @c text
 */
bool source_text_is_index_in_bounds(const SourceText* this, int index);

// for list.h
void source_text_list_destroy(void* this);

#endif //C_COMPILER_SOURCE_TEXT_H