//
// Created by tomer on 14-Oct-26.
//

#include "Context.h"
#include "Parser.h"
#include "List.h"
#include "Token.h"
#include "Utils.h"

#include <assert.h>
#include <stdlib.h>

#include "Arena.h"

struct Parser {
    const List* tokens; ///< a @c List of @c Token . NOT owned by @c Parser
    int cursor;
    Context* context; ///< NOT owned by @c Parser
    bool error;
    int expression_id_counter;
};

// static:

static void parser_set_error(Parser* this) {
    this->error = true;
}
static bool parser_has_error(const Parser* const this) {
    assert(this != NULL);

    return this->error;
}
static void parser_report_diagnostic(Parser* const this, const DiagnosticCode code, const Range range) {
    assert(this != NULL);

    parser_set_error(this);
    context_report_diagnostic(this->context, code, range);
}

static const Token* parser_peek(const Parser* const this) {
    assert(this != NULL);
    return list_at_const(this->tokens, this->cursor);
}
static const Token* parser_pop(Parser* const this) {
    assert(this != NULL);
    const Token* const to_return = list_at_const(this->tokens, this->cursor);
    if (to_return->kind != TOKEN_EOF)
        this->cursor++;
    return to_return;
}
static const Token* parser_expect(Parser* const this, const TokenKind kind) {
    assert(this != NULL);

    if (parser_peek(this)->kind != kind) {
        if (!parser_has_error(this))
            parser_report_diagnostic(
                this,
                DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN,
                parser_peek(this)->range); // TODO: think how to communicate which specific kind was expected.
        return NULL;
    }
    return parser_pop(this);
}
static const Token* parser_expect_type(Parser* const this) {
    assert(this != NULL);

    if (!token_kind_is_type(parser_peek(this)->kind)) {
        if (!parser_has_error(this))
            parser_report_diagnostic(
                this,
                DIAGNOSTIC_CODE_PARSER_MISSING_TYPE_SPECIFIER,
                parser_peek(this)->range);
        return NULL;
    }
    return parser_pop(this);
}

static int parser_generate_next_expression_id(Parser* const this) {
    assert(this != NULL);

    return this->expression_id_counter++;
}
static Arena* parser_get_arena(Parser* const this) {
    assert(this != NULL);

    return this->context->arena;
}

// convertors:

static UnaryExpressionKind token_to_unary_expression_kind(const TokenKind kind) {
    if (kind == TOKEN_TILDE)
        return UNARY_EXPRESSION_KIND_BINARY_NEGATE;
    if (kind == TOKEN_EXCLAMATION)
        return UNARY_EXPRESSION_KIND_LOGICAL_NOT;
    if (kind == TOKEN_PLUS)
        return UNARY_EXPRESSION_KIND_NUMERIC_PLUS;
    if (kind == TOKEN_MINUS)
        return UNARY_EXPRESSION_KIND_NUMERIC_MINUS;

    return UNARY_EXPRESSION_KIND_NONE;
}

// parsing:

static const ExpressionNode*            parser_expression_node_parse(           Parser* const this) {
    assert(this != NULL);
    if (parser_has_error(this))
        return NULL;

    const TokenKind kind = parser_peek(this)->kind;
    // parenthesized expression:
    if (kind == TOKEN_PARENTHESES_LEFT) {
        parser_expect(this, TOKEN_PARENTHESES_LEFT);
        if (parser_has_error(this))
            return NULL;
        const ExpressionNode* const to_return = parser_expression_node_parse(this);
        if (parser_has_error(this))
            return NULL;
        parser_expect(this, TOKEN_PARENTHESES_RIGHT);
        if (parser_has_error(this))
            return NULL;
        return to_return;
    }
    // constant int expression:
    if (kind == TOKEN_CONST_INT) {
        const Token* const token_constant_int = parser_expect(this, TOKEN_CONST_INT);
        if (parser_has_error(this))
            return NULL;

        ExpressionNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (ExpressionNode));
        *to_return = (ExpressionNode){
            .range = token_constant_int->range,
            .id = parser_generate_next_expression_id(this),
            .kind = EXPRESSION_KIND_INT_CONSTANT,
            .as.int_constant = {
                .value = token_constant_int->as.value
            }
        };
        return to_return;
    }
    // unary expression:
    const UnaryExpressionKind unary_expression_kind = token_to_unary_expression_kind(parser_peek(this)->kind);
    if (unary_expression_kind != UNARY_EXPRESSION_KIND_NONE) {
        const Token* const token_unary_punctuator = parser_pop(this);

        const ExpressionNode* const operand = parser_expression_node_parse(this);
        if (parser_has_error(this))
            return NULL;

        ExpressionNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (ExpressionNode));
        *to_return = (ExpressionNode){
            .range = range_union(token_unary_punctuator->range, operand->range),
            .id = parser_generate_next_expression_id(this),
            .kind = EXPRESSION_KIND_UNARY,
            .as.unary = {
                .kind = unary_expression_kind,
                .operand = operand
            }
        };
        return to_return;
    }
    //

    parser_report_diagnostic(
        this,
        DIAGNOSTIC_CODE_PARSER_INVALID_EXPRESSION,
        parser_peek(this)->range);
    return NULL;
}
static const StatementNode*             parser_statement_node_parse(            Parser* const this) {
    assert(this != NULL);
    if (parser_has_error(this))
        return NULL;

    const TokenKind kind = parser_peek(this)->kind;
    // return statement:
    if (kind == TOKEN_KW_RETURN) {
        const Token* const token_return = parser_expect(this, TOKEN_KW_RETURN);
        if (parser_has_error(this))
            return NULL;
        const ExpressionNode* const return_expression = parser_expression_node_parse(this);
        if (parser_has_error(this))
            return NULL;
        const Token* const token_semicolon = parser_expect(this, TOKEN_SEMICOLON);
        if (parser_has_error(this))
            return NULL;

        StatementNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (StatementNode));
        *to_return = (StatementNode){
            .range = range_union(token_return->range, return_expression->range, token_semicolon->range),
            .kind = STATEMENT_KIND_RETURN,
            .as.return_statement = {
                .expression = return_expression
            }
        };
        return to_return;
    }
    // compound statement:
    if (kind == TOKEN_BRACE_LEFT) {
        const Token* const token_brace_left = parser_expect(this, TOKEN_BRACE_LEFT);
        if (parser_has_error(this))
            return NULL;

        BlockItem dummy = {.next = NULL};
        for (BlockItem* curr = &dummy; parser_peek(this)->kind != TOKEN_BRACE_RIGHT; curr = curr->next) {
            // TODO: in the future, add a declaration option
            const StatementNode* const block_statement = parser_statement_node_parse(this);
            if (parser_has_error(this))
                return NULL;

            BlockItem* const next = arena_malloc(parser_get_arena(this), sizeof (BlockItem));
            *next = (BlockItem){
                .kind = BLOCK_ITEM_KIND_STATEMENT,
                .as.statement = block_statement,
                .next = NULL
            };

            curr->next = next;
        }
        const Token* const token_brace_right = parser_expect(this, TOKEN_BRACE_RIGHT); // pop the '}' token
        if (parser_has_error(this))
            return NULL;

        StatementNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (StatementNode));
        *to_return = (StatementNode){
            .range = range_union(token_brace_left->range, token_brace_right->range),
            .kind = STATEMENT_KIND_COMPOUND,
            .as.compound_statement = {.head = dummy.next}
        };
        return to_return;
    }

    parser_report_diagnostic(
        this,
        DIAGNOSTIC_CODE_PARSER_INVALID_STATEMENT,
        parser_peek(this)->range);
    return NULL;
}
static const DeclaratorNode*            parser_declarator_node_parse(           Parser* const this, const Token base_type) {
    assert(this != NULL);
    if (parser_has_error(this))
        return NULL;
    assert(token_kind_is_type(base_type.kind));

    // TODO: add support for pointers and arrays.
    const Token* const token_identifier = parser_expect(this, TOKEN_IDENT);
    if (parser_has_error(this))
        return NULL;

    parser_expect(this, TOKEN_PARENTHESES_LEFT); // TODO: add support for variable declarations, remove in slice 10
    if (parser_has_error(this))
        return NULL;
    parser_expect(this, TOKEN_KW_VOID);
    if (parser_has_error(this))
        return NULL;
    const Token* const token_parenthesis_right = parser_expect(this, TOKEN_PARENTHESES_RIGHT);
    if (parser_has_error(this))
        return NULL;

    DeclaratorNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (DeclaratorNode));
    *to_return = (DeclaratorNode){
        .base_type = base_type,
        .range = range_union(token_identifier->range, token_parenthesis_right->range),
        .type = parser_type_create_function(parser_get_arena(this), type_get_singleton(token_kind_get_type_kind(base_type.kind)), NULL, 0),
        .identifier = *token_identifier
    };
    return to_return;
}
static const FunctionDefinitionNode*    parser_function_definition_node_parse(  Parser* const this, const DeclaratorNode* const declarator_node) {
    assert(this != NULL);
    if (parser_has_error(this))
        return NULL;
    assert(declarator_node != NULL);

    const StatementNode* const body = parser_statement_node_parse(this);
    if (parser_has_error(this))
        return NULL;
    if (body->kind != STATEMENT_KIND_COMPOUND) {
        parser_report_diagnostic(
            this,
            DIAGNOSTIC_CODE_PARSER_EXPECTED_COMPOUND_STATEMENT,
            body->range);
        return NULL;
    }

    FunctionDefinitionNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (FunctionDefinitionNode));
    *to_return = (FunctionDefinitionNode){
        .range = range_union(declarator_node->base_type.range, body->range),
        .declarator = declarator_node,
        .parameters = NULL,     // TODO: add support to parameter lists.
        .body = body
    };
    return to_return;
}
static const DeclarationNode*           parser_declaration_node_parse(          Parser* const this, const DeclaratorNode* const first_declarator) {
    assert(this != NULL);
    if (parser_has_error(this))
        return NULL;
    assert(first_declarator != NULL);

    DeclaratorItem* const head = arena_malloc(parser_get_arena(this), sizeof (DeclaratorItem));
    *head = (DeclaratorItem){
        .declarator = first_declarator,
        .initializer = NULL,
        .next = NULL
    };

    for (DeclaratorItem* curr = head; parser_peek(this)->kind == TOKEN_COMMA; curr = curr->next) {
        parser_expect(this, TOKEN_COMMA);

        const DeclaratorNode* const next_declarator_node = parser_declarator_node_parse(this, first_declarator->base_type);
        if (parser_has_error(this))
            return NULL;

        curr->next = arena_malloc(parser_get_arena(this), sizeof (DeclaratorItem));
        *(curr->next) = (DeclaratorItem){
            .declarator = next_declarator_node,
            .initializer = NULL,
            .next = NULL
        };
    }
    const Token* const token_semicolon = parser_expect(this, TOKEN_SEMICOLON);
    if (parser_has_error(this))
        return NULL;

    DeclarationNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (DeclarationNode));
    *to_return = (DeclarationNode){
        .range = range_union(first_declarator->base_type.range, token_semicolon->range),
        .head = head
    };
    return to_return;
}
static const TranslationUnitNode*       parser_translation_unit_node_parse(     Parser* const this) {
    assert(this != NULL);
    if (parser_has_error(this))
        return NULL;

    if (parser_peek(this)->kind == TOKEN_EOF) {
        parser_report_diagnostic(
            this,
            DIAGNOSTIC_CODE_PARSER_MISSING_EXTERNAL_DEFINITION,
            parser_peek(this)->range);
        return NULL;
    }

    const Range start = parser_peek(this)->range;
    ExternalDeclarationItem dummy = { .next = NULL };
    for (ExternalDeclarationItem* curr = &dummy; parser_peek(this)->kind != TOKEN_EOF; curr = curr->next) {
        const Token* const base_type = parser_expect_type(this);
        if (parser_has_error(this))
            return NULL;

        const DeclaratorNode* const declarator_node = parser_declarator_node_parse(this, *base_type);
        if (parser_has_error(this))
            return NULL;

        // check which external declaration it is:
        const TokenKind kind = parser_peek(this)->kind;
        if (kind == TOKEN_BRACE_LEFT) { // function definition
            if (declarator_node->type->kind != TYPE_KIND_FUNCTION) {
                // TODO: issue diagnostic - expected ';' (because it cannot be a FUNCTION definition)
            }

            const FunctionDefinitionNode* const function_definition_node = parser_function_definition_node_parse(this, declarator_node);
            if (parser_has_error(this))
                return NULL;

            curr->next = arena_malloc(parser_get_arena(this), sizeof (ExternalDeclarationItem));
            *(curr->next) = (ExternalDeclarationItem){
                .kind = EXTERNAL_DECLARATION_ITEM_KIND_FUNCTION,
                .as.external_function_definition = function_definition_node,
                .next = NULL
            };
        }
        else if (   kind == TOKEN_SEMICOLON ||
                    kind == TOKEN_COMMA     ||
                    kind == TOKEN_EQUALS) { // declaration
            parser_report_diagnostic( // TODO: remove when adding support to declarations
                this,
                DIAGNOSTIC_CODE_PARSER_EXPECTED_SEMICOLON,
                parser_peek(this)->range);
            return NULL;
        }
        else {
            parser_report_diagnostic(
                this,
                DIAGNOSTIC_CODE_PARSER_INVALID_EXTERNAL_DEFINITION,
                parser_peek(this)->range);
            return NULL;
        }
    }
    const Range end = parser_peek(this)->range;

    TranslationUnitNode* const to_return = arena_malloc(parser_get_arena(this), sizeof (TranslationUnitNode));
    *to_return = (TranslationUnitNode){
        .range = range_union(start, end),
        .head = dummy.next
    };
    return to_return;
}

// API:

Parser* parser_create(const List* const tokens, Context* const context)  {
    assert(tokens != NULL);
    assert(!list_is_empty(tokens));
    assert(((const Token*)list_at_const(tokens, list_get_size(tokens) - 1))->kind == TOKEN_EOF);

    Parser* const parser = utils_xmalloc(sizeof (Parser));
    *parser = (Parser){
        .tokens = tokens,
        .cursor = 0,
        .context = context,
        .error = false,
        .expression_id_counter = 0
    };
    return parser;
}
void parser_destroy(Parser* const this) {
    assert(this != NULL);
    free(this);
}

const TranslationUnitNode* parser_parse(Parser* this) {
    assert(this != NULL);
    if (parser_has_error(this))
        return NULL;

    return parser_translation_unit_node_parse(this);
}

int parser_get_expression_id_count(const Parser* const this) {
    assert(this != NULL);

    return this->expression_id_counter;
}
