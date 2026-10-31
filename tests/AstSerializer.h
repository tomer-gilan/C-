//
// Created by tomer on 21-Oct-26. Moved to tests/ and taken over by Claude on 31-Oct-26.
//
// The AST dump: prints a translation unit as the S-expressions of docs/specifications/ast-dump-format.md, so tests can
// compare an AST against a string. A test tool, not part of the compiler.
//

#ifndef C_COMPILER_AST_SERIALIZER_H
#define C_COMPILER_AST_SERIALIZER_H
#include "ExternalDefinition.h"
#include "Sema.h"
#include "Text.h"

typedef struct Context Context;
typedef struct AstSerializer AstSerializer;

/** Whitespace only: both modes print the same tokens (ast-dump-format.md, "Whitespace: FLAT and NESTED"). */
typedef enum {
    AST_SERIALIZATION_MODE_FLAT,    ///< one line, single spaces, no trailing newline
    AST_SERIALIZATION_MODE_NESTED,  ///< one line per function definition, declaration and statement, 2-space indents

    AST_SERIALIZATION_MODE__COUNT,
} AstSerializationMode;

/**
 * @param context              where identifiers' spellings are looked up. Not owned.
 * @param expression_semantics sema's table, indexed by expression id, for the typed dump (every expression gets
 *                             @c :T ); @c NULL for the untyped dump. Not owned.
 */
AstSerializer* ast_serializer_create(
    const Context* context,
    AstSerializationMode serialization_mode,
    const ExpressionSemantics* expression_semantics);
void ast_serializer_destroy(AstSerializer* this);

/** @return a new @c Text holding the dump of @b translation_unit ; the caller owns it. Can be called more than once. */
Text* ast_serializer_serialize(AstSerializer* this, const TranslationUnitNode* translation_unit);

#endif //C_COMPILER_AST_SERIALIZER_H
