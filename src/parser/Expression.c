//
// Created by tomer on 14-Oct-26.
//

#include "Expression.h"
#include "Parser.h"
#include "Arena.h"

#include <assert.h>

static ExpressionNode* expression_int_constant_parse(Parser* const parser) {
    if (parser_has_error(parser))
        return NULL;

    const Token* const token_constant_int = parser_expect(parser, TOKEN_CONST_INT);
    if (parser_has_error(parser))
        return NULL;
    ExpressionNode* to_return = arena_allocate(parser_get_arena(parser), (sizeof (ExpressionNode)));
    *to_return = (ExpressionNode){
        .range = token_constant_int->range,
        .id = parser_generate_next_expression_id(parser),
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

    parser_report_diagnostic(
        parser,
        DIAGNOSTIC_CODE_PARSER_INVALID_EXPRESSION,
        parser_peek(parser)->range);
    return NULL;
}

ExpressionNode* expression_node_parse(Parser* parser) {
    if (parser_has_error(parser))
        return NULL;

    return primary_expression_parse(parser); // TODO: later, add a switch when other expressions are parsed.
}
