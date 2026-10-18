//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_PARSER_H
#define C_COMPILER_PARSER_H

typedef struct Parser Parser;
typedef struct Arena Arena;
typedef struct Context Context;
#include "ExternalDefinition.h"
#include "List.h"
#include "Token.h"
#include "Diagnostic.h"

Parser* parser_create(const List* tokens, Context* context);
void parser_destroy(Parser* this);

const Token* parser_peek(const Parser* this);
const Token* parser_pop(Parser* this);
const Token* parser_expect(Parser* this, TokenKind kind);

bool parser_has_error(const Parser* this);
void parser_report_diagnostic(Parser* this, DiagnosticCode code, Range range);

int parser_generate_next_expression_id(Parser* this);
int parser_get_expression_id_count(const Parser* this);
Arena* parser_get_arena(Parser* this);
/**
 * @attention shares the types with the created function type!
 */
const Type* parser_get_function_type_from_declarators(Parser* this, const Declarator* declarators, int count, const Type* return_type);

const TranslationUnitNode* parser_parse(Parser* this);

#endif //C_COMPILER_PARSER_H
