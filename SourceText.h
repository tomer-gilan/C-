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
 * @param path the path to the source file.
 * @return a @c SourceText handle
 * @retval NULL if @b path is invalid
 */
SourceText* source_file_create_from_path(const char* path);
/**
 * Creates a new @c SourceText from a given @b string.
 * @param string the source string
 * @return a @c SourceText handle
 * @retval NULL if @b path is invalid
 */
SourceText* source_file_create_from_string(const char* string);
/**
 * Destroys the @c SourceText .
 * @param this handle to the @c SourceText
 */
void source_file_destroy(SourceText* this);
/**
 * checks if the @b index is in the bounds of @c text
 */
bool source_file_is_index_in_bounds(const SourceText* this, int index);

// for list.h
void source_file_list_destroy(void* this);

#endif //C_COMPILER_SOURCE_TEXT_H