//
// Created by tomer on 14-Oct-26.
//

#include "ExternalDefinition.h"
#include "Parser.h"
#include "Arena.h"

#include <assert.h>
#include <stdlib.h>

static ExternalDefinitionNode* external_function_definition_parse(Parser* parser, const Token* const type_specifier, const Token* const identifier) {
    if (parser_has_error(parser))
        return NULL;

    const Token* const token_pare_left = parser_expect(parser,TOKEN_PARENTHESES_LEFT);
    const Token* const token_void =parser_expect(parser, TOKEN_KW_VOID);
    const Token* const token_pare_right =parser_expect(parser,TOKEN_PARENTHESES_RIGHT);

    StatementNode* const body = compound_statement_parse(parser);
    if (parser_has_error(parser))
        return NULL;
    if (body->kind != STATEMENT_KIND_COMPOUND) {
        // TODO: issue a diagnostic - expected compound statement
        parser_set_error(parser);
    }

    ExternalDefinitionNode* to_return = arena_allocate(parser_get_arena(parser), sizeof (ExternalDefinitionNode));
    *to_return = (ExternalDefinitionNode){
        .range = range_union(type_specifier->range, identifier->range, token_pare_left->range, token_void->range, token_pare_right->range, body->range),
        .kind = EXTERNAL_DEFINITION_KIND_FUNCTION,
        .as.external_function_definition = {
            .name_identifier_id = identifier->as.id,
            .body = body
        },
        .next = NULL
    };
    return to_return;
}
static ExternalDefinitionNode* external_definition_node_parse(Parser* parser) {
    if (parser_has_error(parser))
        return NULL;

    const Token* const type_specifier = parser_expect(parser, TOKEN_KW_INT);
    const Token* const identifier = parser_expect(parser, TOKEN_IDENT);

    switch (parser_peek(parser)->kind) {
        case TOKEN_PARENTHESES_LEFT: // function declaration
            return external_function_definition_parse(parser, type_specifier, identifier);
        default:
            // TODO: issue a diagnostic - unsupported external definition
            parser_set_error(parser);
            return NULL; // TODO: change to handle better
    }
}
TranslationUnitNode* translation_unit_node_parse(Parser* parser) {
    if (parser_has_error(parser))
        return NULL;

    if (parser_peek(parser)->kind == TOKEN_EOF) {
        // TODO: issue a diagnostic - no external definitions found
        parser_set_error(parser);
        return NULL;
    }

    bool is_first_iteration = true;
    Range node_union_range = RANGE_EMPTY;
    ExternalDefinitionNode dummy = {.next = NULL};
    for (ExternalDefinitionNode* curr = &dummy; parser_peek(parser)->kind != TOKEN_EOF; curr = curr->next) {
        curr->next = external_definition_node_parse(parser);
        if (parser_has_error(parser))
            return NULL;

        if (is_first_iteration) {
            is_first_iteration = false;
            node_union_range = curr->next->range;
        }
        else
            node_union_range = range_union(node_union_range, curr->next->range);
    }

    TranslationUnitNode* to_return = arena_allocate(parser_get_arena(parser), sizeof (TranslationUnitNode));
    *to_return = (TranslationUnitNode){
        .range = node_union_range,
        .head = dummy.next
    };
    return to_return;
}
