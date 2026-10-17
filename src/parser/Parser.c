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

const Token* parser_peek(const Parser* const this) {
    assert(this != NULL);
    return list_at_const(this->tokens, this->cursor);
}
const Token* parser_pop(Parser* const this) {
    assert(this != NULL);
    const Token* const to_return = list_at_const(this->tokens, this->cursor);
    if (to_return->kind != TOKEN_EOF)
        this->cursor++;
    return to_return;
}
const Token* parser_expect(Parser* const this, const TokenKind kind) {
    if (parser_peek(this)->kind != kind) {
        if (!parser_has_error(this))
            parser_report_diagnostic(this, DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, parser_peek(this)->range); // TODO: think how to communicate which specific kind was expected.
        return NULL;
    }
    return parser_pop(this);
}
Arena* parser_get_arena(Parser* const this) {
    return this->context->arena;
}
void parser_set_error(Parser* this) {
    this->error = true;
}
bool parser_has_error(const Parser* const this) {
    return this->error;
}
void parser_report_diagnostic(Parser* const this, const DiagnosticCode code, const Range range) {
    assert(this != NULL);
    parser_set_error(this);
    context_report_diagnostic(this->context, code, range);
}
int parser_generate_next_expression_id(Parser* const this) {
    return this->expression_id_counter++;
}
int parser_get_expression_id_count(const Parser* const this) {
    return this->expression_id_counter;
}
const Type* parser_get_function_type_from_declarators(Parser* const this, const Declarator* const declarators, const int count, const Type* const return_type) {
    assert(count >= 0);
    const Type** parameters_type = arena_allocate(parser_get_arena(this), (int)sizeof (Type*) * count);
    for (int i = 0; i < count; ++i) {
        parameters_type[i] = declarators[i].type;
    }

    Type* const to_return = arena_allocate(parser_get_arena(this), sizeof (Type));
    *to_return = (Type){
        .kind = TYPE_KIND_FUNCTION,
        .as.function = {
            .return_type = return_type,
            .parameters_type = parameters_type,
            .parameters_count = count
        }
    };
    return to_return;
}
TranslationUnitNode* parser_parse(Parser* this) {
    if (parser_has_error(this))
        return NULL;

    return translation_unit_node_parse(this);
}
