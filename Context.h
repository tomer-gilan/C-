//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_CONTEXT_H
#define C_COMPILER_CONTEXT_H

#include "List.h"
#include "Location.h"

typedef struct {
    List* identifiers_table;
    List* string_literals_table;
    List* source_file_table;
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