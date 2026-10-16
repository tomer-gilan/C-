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
    if (parser_has_error(parser))
        return NULL;
    const Token* const token_void = parser_expect(parser, TOKEN_KW_VOID);
    if (parser_has_error(parser))
        return NULL;
    const Token* const token_pare_right = parser_expect(parser,TOKEN_PARENTHESES_RIGHT);
    if (parser_has_error(parser))
        return NULL;
    StatementNode* const body = compound_statement_parse(parser);
    if (parser_has_error(parser))
        return NULL;

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
    if (parser_has_error(parser))
        return NULL;
    const Token* const identifier = parser_expect(parser, TOKEN_IDENT);
    if (parser_has_error(parser))
        return NULL;

    const TokenKind kind = parser_peek(parser)->kind;
    if (kind == TOKEN_PARENTHESES_LEFT)
        return external_function_definition_parse(parser, type_specifier, identifier);
    parser_report_diagnostic(
        parser,
        DIAGNOSTIC_CODE_PARSER_EXPECTED_EXTERNAL_FUNCTION_DEFINITION, // for now, only function definitions are supported for external definitions
        parser_peek(parser)->range);
    return NULL;
}
TranslationUnitNode* translation_unit_node_parse(Parser* parser) {
    if (parser_has_error(parser))
        return NULL;

    if (parser_peek(parser)->kind == TOKEN_EOF) {
        parser_report_diagnostic(
            parser,
            DIAGNOSTIC_CODE_PARSER_MISSING_EXTERNAL_DEFINITION,
            parser_peek(parser)->range);
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
