//
// Created by tomer on 14-Oct-26.
//

#include "Expression.h"
#include "Parser.h"
#include "Arena.h"

#include <assert.h>

static ExpressionNode* expression_int_constant_parse(Parser* const parser) {
    const Token* const token_constant_int = parser_expect(parser, TOKEN_CONST_INT);
    ExpressionNode* to_return = arena_allocate(parser_get_arena(parser), (sizeof (ExpressionNode)));
    *to_return = (ExpressionNode){
        .range = token_constant_int->range,
        .kind = EXPRESSION_KIND_INT_CONSTANT,
        .as.expression_int_constant = { .value = token_constant_int->as.value }
    };
    return to_return;
}

static ExpressionNode* primary_expression_parse(Parser* parser) {
    if (parser_has_error(parser))
        return NULL;

    const TokenKind kind = parser_peek(parser)->kind;
    if (kind == TOKEN_CONST_INT)
        return expression_int_constant_parse(parser);
    // TODO: issue diagnostic - invalid return expression
    parser_set_error(parser);
    return NULL;
}

ExpressionNode* expression_node_parse(Parser* parser) {
    if (parser_has_error(parser))
        return NULL;
    return primary_expression_parse(parser); // TODO: later, add a switch when other expressions are parsed.
}
