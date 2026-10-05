//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_CONTEXT_H
#define C_COMPILER_CONTEXT_H

#include "List.h"
#include "SourceText.h"
#include "Text.h"
#include "TextView.h"

typedef struct {
    List* identifiers_table;        ///< @c List of @c TextView
    List* string_literals_table;    ///< @c List of @c char* (@c string)
    List* source_text_table;        ///< @c List of @c SourceText
} Context;

/**
 * Creates a new @c Context .
 * @warning ownership of @b source is transferred to the callee.
 * @param source a @c SourceText containing the source code.
 * @return a @c Context handle
 */
Context* context_create(SourceText* source);
/**
 * Destroys the @c Context .
 * @param this handle to the @c Context
 */
void context_destroy(Context* this);
/**
 * retrieves @c SourceText at @b index.
 * @warning ownership of @c SourceText does not transfer.
 */
SourceText* context_source_text_at(Context* this, int index);
/**
 * @c const version of @c context_source_text_at
 */
const SourceText* context_source_text_at_const(const Context* this, int index);
/**
 * caches the identifier spelling.
 * @return the id of the cached spelling.
 * @note if the spelling is already cached, the existing id is returned.
 */
int context_intern_identifier(Context* this, TextView spelling);
/**
 * caches the string literal spelling.
 * @return the id of the cached spelling.
 * @note if the spelling is already cached, the existing id is returned.
 */
int context_intern_string_literal(Context* this, Text* spelling);

#endif //C_COMPILER_CONTEXT_H