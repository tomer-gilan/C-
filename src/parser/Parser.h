//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_PARSER_H
#define C_COMPILER_PARSER_H

typedef struct Parser Parser;
typedef struct Context Context;
#include "ExternalDefinition.h"
#include "List.h"

Parser* parser_create(const List* tokens, Context* context);
void parser_destroy(Parser* this);

const TranslationUnitNode* parser_parse(Parser* this);

int parser_get_expression_id_count(const Parser* this);

#endif //C_COMPILER_PARSER_H
