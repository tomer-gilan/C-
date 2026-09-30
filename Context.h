//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_CONTEXT_H
#define C_COMPILER_CONTEXT_H

#include <assert.h>

#include "List.h"
#include "SourceFile.h"

typedef struct {
    List* identifiers_table;        ///< @c List of @c StringView
    List* string_literals_table;    ///< @c List of @c char* (@c string)
    List* source_file_table;        ///< @c List of @c SourceFile
} Context;

/**
 * Creates a new @c Context .
 * @param source_file_path the path to the source file.
 * @return a @c Context handle
 * @retval NULL if @b source_file_path is invalid
 */
Context* context_create(const char* source_file_path);
/**
 * Destroys the @c Context .
 * @param this handle to the @c Context
 */
void context_destroy(Context* this);

#endif //C_COMPILER_CONTEXT_H