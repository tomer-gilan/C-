//
// Created by tomer on 14-Oct-26.
//

#include "Parser.h"
#include "Statement.h"
#include "Arena.h"

#include <assert.h>
#include <stdlib.h>

/**
 * @attention assumes @b block_item 's kind is valid AND supported!!
 */
static Range block_item_get_range(const BlockItem* const block_item) {
    assert(block_item != NULL);

    switch (block_item->kind) {
        case BLOCK_ITEM_KIND_STATEMENT:
            return block_item->as.statement->range;
        case BLOCK_ITEM_KIND_DECLARATION:
        default:
            return RANGE_EMPTY; // should never reach this case.
    }
}

static BlockItem* block_item_parse(Parser* parser) {
    if (parser_has_error(parser))
        return NULL;

    // TODO: in the future, add a declaration option
    BlockItem* const to_return = arena_allocate(parser_get_arena(parser), sizeof (BlockItem));
    *to_return = (BlockItem){
        .kind = BLOCK_ITEM_KIND_STATEMENT,
        .as.statement = statement_node_parse(parser),
        .next = NULL
    };
    return to_return;
}
static StatementNode* return_statement_parse(Parser* const parser) {
    assert(parser_peek(parser)->kind == TOKEN_KW_RETURN);
    if (parser_has_error(parser))
        return NULL;

    const Token* const return_token = parser_expect(parser, TOKEN_KW_RETURN);
    ExpressionNode* const return_expression = expression_node_parse(parser);
    if (parser_has_error(parser))
        return NULL;
    const Token* const token_semicolon = parser_expect(parser, TOKEN_SEMICOLON);
    if (parser_has_error(parser))
        return NULL;

    StatementNode* const to_return = arena_allocate(parser_get_arena(parser), sizeof (StatementNode));
    *to_return = (StatementNode){
        .range = range_union(return_token->range, return_expression->range, token_semicolon->range),
        .kind = STATEMENT_KIND_RETURN,
        .as.return_statement = { .expression = return_expression }
    };
    return to_return;
}
StatementNode* compound_statement_parse(Parser* const parser) {
    assert(parser_peek(parser)->kind == TOKEN_BRACE_LEFT);

    if (parser_has_error(parser))
        return NULL;

    const Token* const token_brace_left = parser_pop(parser); // pops the '{' token
    Range node_union_range = token_brace_left->range;

    BlockItem dummy = {.next = NULL};
    for (BlockItem* curr = &dummy; parser_peek(parser)->kind != TOKEN_BRACE_RIGHT; curr = curr->next) {
        curr->next = block_item_parse(parser);
        if (parser_has_error(parser))
            return NULL;

        node_union_range = range_union(node_union_range, block_item_get_range(curr->next));
    }
    const Token* const token_brace_right = parser_expect(parser, TOKEN_BRACE_RIGHT); // pop the '}' token
    node_union_range = range_union(node_union_range, token_brace_right->range);

    StatementNode* const to_return = arena_allocate(parser_get_arena(parser), sizeof (StatementNode));
    *to_return = (StatementNode){
        .range = node_union_range,
        .kind = STATEMENT_KIND_COMPOUND,
        .as.compound_statement = {.head = dummy.next}
    };
    return to_return;
}
StatementNode* statement_node_parse(Parser* const parser) {
    if (parser_has_error(parser))
        return NULL;

    switch (parser_peek(parser)->kind) {
        case TOKEN_KW_RETURN: // return-statement
            return return_statement_parse(parser);
        case TOKEN_BRACE_LEFT: // compound-statement
            return compound_statement_parse(parser);
        default:
            // TODO: issue diagnostic - invalid statement
            parser_set_error(parser);
            return NULL;
    }
}
