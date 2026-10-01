//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_SOURCEFILE_H
#define C_COMPILER_SOURCEFILE_H

#include <stdbool.h>

typedef struct {
    char* name; ///< the name of the file, i.e. path==full/path/to/file_name.c and name==file_name.c
    char* text;
    int length;
} SourceFile;

/**
 * Creates a new @c SourceFile .
 * @param path the path to the source file.
 * @return a @c SourceFile handle
 * @retval NULL if @b path is invalid
 */
SourceFile* source_file_create(const char* path);
/**
 * Destroys the @c SourceFile .
 * @param this handle to the @c SourceFile
 */
void source_file_destroy(SourceFile* this);
/**
 * checks if the @b index is in the bounds of @c text
 */
bool source_file_is_index_in_bounds(const SourceFile* this, int index);

// for list.h
void source_file_list_destroy(void* this);

#endif //C_COMPILER_SOURCEFILE_H