//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_CONTEXT_H
#define C_COMPILER_CONTEXT_H

#include "List.h"
#include "SourceText.h"

typedef struct {
    List* identifiers_table;        ///< @c List of @c StringView
    List* string_literals_table;    ///< @c List of @c char* (@c string)
    List* source_file_table;        ///< @c List of @c SourceText
} Context;

/**
 * Creates a new @c Context .
 * @param source a @c SourceFile containing the source code.
 * @return a @c Context handle
 * @retval NULL if @b source_file_path is invalid
 */
Context* context_create(SourceText* source);
/**
 * Destroys the @c Context .
 * @param this handle to the @c Context
 */
void context_destroy(Context* this);
/**
 * retrieves @c SrouceFile at @b index.
 * @warning ownership of @c SourceText does not transfer.
 */
SourceText* context_source_file_at(Context* this, int index);
/**
 * @c const version of @c context_source_file_at
 */
const SourceText* context_source_file_at_const(const Context* this, int index);

#endif //C_COMPILER_CONTEXT_H