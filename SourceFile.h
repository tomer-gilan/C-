//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_SOURCEFILE_H
#define C_COMPILER_SOURCEFILE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    char* name;
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

#endif //C_COMPILER_SOURCEFILE_H