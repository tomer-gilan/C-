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

Parser* parser_create(const List* tokens, Context* context);
void parser_destroy(Parser* this);
const Token* parser_peek(const Parser* this);
const Token* parser_pop(Parser* this);
const Token* parser_expect(Parser* this, TokenKind kind);
Arena* parser_get_arena(Parser* this);
void parser_set_error(Parser* this);
bool parser_has_error(const Parser* this);

TranslationUnitNode* parser_parse(Parser* this);

#endif //C_COMPILER_PARSER_H
