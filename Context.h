//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_CONTEXT_H
#define C_COMPILER_CONTEXT_H

#include "List.h"
#include "SourceText.h"
#include "Text.h"
#include "TextView.h"
#include "Diagnostic.h"

typedef struct {
    bool pedantic; ///< interpret every warning as an error
} CompilationFlags;

CompilationFlags compilation_flags_from_arguments(int argc, const char* const* argv);

CompilationFlags compilation_flags_basic();

typedef struct Context {
    List* identifiers_table;        ///< @c List of @c TextView
    List* string_literals_table;    ///< @c List of @c Text
    List* source_text_table;        ///< @c List of @c SourceText
    List* diagnostic_report_table; ///< @c List of @c Diagnostic

    CompilationFlags flags;
} Context;

/**
 * Creates a new @c Context .
 * @warning ownership of @b source is transferred to the callee.
 * @param source a @c SourceText containing the source code.
 * @param flags the flags needed for the compilation process.
 * @return a @c Context handle
 */
Context *context_create(SourceText* source, CompilationFlags flags);

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
 * returns a @c TextView based on the given @c Range
 */
TextView context_range_to_text_view(const Context * this, Range range);
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
 * @attention ownership of @b text remains with the caller
 */
int context_intern_string_literal(Context * this, const Text * text);
/**
 * caches the diagnostic.
 */
void context_report_diagnostic(Context * this, Diagnostic report);


#endif //C_COMPILER_CONTEXT_H
