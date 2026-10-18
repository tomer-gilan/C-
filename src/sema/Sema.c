//
// Created by tomer on 16-Oct-26.
//

#include "Sema.h"
#include "Context.h"
#include "ScopeStack.h"
#include "Diagnostic.h"

#include <assert.h>
#include <stdlib.h>

struct Sema {
    Context* context;
    const TranslationUnitNode* ast;
    bool error;                                 ///< error flag
    ScopeStack* scope_stack;

    ExpressionSemantics* expressions_semantics; ///< keyed by @c expression.id
    int expressions_count;
};

static void sema_set_error(Sema* this) {
    assert(this != NULL);
    this->error = true;
}
static bool sema_has_error(const Sema* const this) {
    assert(this != NULL);
    return this->error;
}
static void sema_report_diagnostic(Sema* const this, const DiagnosticCode code, const Range range) {
    assert(this != NULL);
    sema_set_error(this);
    context_report_diagnostic(this->context, code, range);
}

/**
 * interns an @c ExpressionNode into an @c ExpressionSemantics entry in @c expressions_semantics
 * @attention can only be called once per expression (asserts so)
 */
static void sema_record_expression(Sema* const this, int const id, const Type* const type, const Symbol* const identifier_symbol) {
    assert(this != NULL);
    assert(0 <= id && id < this->expressions_count);

    this->expressions_semantics[id] = (ExpressionSemantics){
        .type = type,
        .identifier_symbol = identifier_symbol
    };
}
/**
 * illustration: @c ScopeStack [@c identifier.as.id ] = @c Symbol* .
 *
 * interns a @c Symbol* into the @c ScopeStack , keyed by the @c Declarator 's @c identifier.as.id
 */
static void sema_record_declarator(Sema* const this, const Declarator* const declarator) {
    assert(this != NULL);
    assert(declarator != NULL);
    const int identifier_id = declarator->identifier.as.id;
    assert(0 <= identifier_id && identifier_id < list_get_size(this->context->identifiers_table));

    scope_stack_set(this->scope_stack, identifier_id, symbol_allocate(this->context->arena, declarator));
}

Sema* sema_create(Context* const context, const TranslationUnitNode* const ast, const int expression_count) {
    assert(context != NULL);
    assert(ast != NULL);
    assert(expression_count >= 0);

    Sema* to_return = utils_xmalloc(sizeof(Sema));
    *to_return = (Sema){
        .context = context,
        .ast = ast,
        .error = false,
        .scope_stack = scope_stack_create(list_get_size(context->identifiers_table)),
        .expressions_semantics = arena_calloc(context->arena, sizeof(ExpressionSemantics), expression_count),
        .expressions_count = expression_count
    };
    return to_return;
}
void sema_destroy(Sema* const this) {
    assert(this != NULL);

    scope_stack_destroy(this->scope_stack);
    free(this);
}

static void sema_analyze_statement(Sema* this, const StatementNode* statement_node); // forward declaration
// analyzing logic:
static void sema_analyze_expression(Sema* const this, const ExpressionNode* const expression_node) {
    assert(this != NULL);

    if (sema_has_error(this))
        return;

    // finds the expression's Type:
    const Type* type = NULL;
    assert(this->expressions_semantics[expression_node->id].type == NULL);
    switch (expression_node->kind) {
        case EXPRESSION_KIND_INT_CONSTANT:
            type = type_get_int();
            break;
        case EXPRESSION_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }

    // SYMBOL
    const Symbol* identifier_symbol = NULL;
    // TODO: check that the expression kind is identifier. then assert the symbol is NULL. then fetch the symbol from ScopeStack. if NULL, issue diagnostic (variable used but not declared), else, assign.

    sema_record_expression(this, expression_node->id, type, identifier_symbol);
}
static void sema_analyze_block_item(Sema* const this, const BlockItem* const block_item) {
    assert(this != NULL);

    if (sema_has_error(this))
        return;
    switch (block_item->kind) {
        case BLOCK_ITEM_KIND_STATEMENT:
            sema_analyze_statement(this, block_item->as.statement);
            break;
        case BLOCK_ITEM_KIND_DECLARATION: // TODO: add support in future slices
        case BLOCK_ITEM_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void sema_analyze_compound_statement(Sema* const this, const CompoundStatement* const compound_statement) {
    assert(this != NULL);

    if (sema_has_error(this))
        return;
    for (const BlockItem* curr = compound_statement->head; curr != NULL; curr = curr->next) {
        sema_analyze_block_item(this, curr);
    }
}
static void sema_analyze_statement(Sema* const this, const StatementNode* const statement_node) {
    assert(this != NULL);

    if (sema_has_error(this))
        return;
    switch (statement_node->kind) {
        case STATEMENT_KIND_RETURN:
            sema_analyze_expression(this, statement_node->as.return_statement.expression);
            break;
        case STATEMENT_KIND_COMPOUND:
            sema_analyze_compound_statement(this, &(statement_node->as.compound_statement));
            break;
        case STATEMENT_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void sema_analyze_external_definition_function(Sema* const this, const ExternalDefinitionFunction* const external_definition_function) {
    assert(this != NULL);

    if (sema_has_error(this))
        return;
    // TODO: in the future, also check that the function return type matches the actually returned expression.

    const int identifier_id = external_definition_function->declarator.identifier.as.id;
    if (scope_stack_was_identifier_declared_in_current_scope(this->scope_stack, identifier_id)) {
        sema_report_diagnostic(this, DIAGNOSTIC_CODE_SEMA_SYMBOL_ALREADY_DEFINED, external_definition_function->declarator.identifier.range);
        return;
    }
    sema_record_declarator(this, &(external_definition_function->declarator));

    sema_analyze_statement(this, external_definition_function->body);
}
static void sema_analyze_external_definition(Sema* const this, const ExternalDefinitionNode* external_definition_node) {
    assert(this != NULL);

    if (sema_has_error(this))
        return;
    switch (external_definition_node->kind) {
        case EXTERNAL_DEFINITION_KIND_FUNCTION:
            sema_analyze_external_definition_function(this, &(external_definition_node->as.external_function_definition));
            break;
        case EXTERNAL_DEFINITION_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
SemaResult sema_analyze(Sema* const this) {
    assert(this != NULL);

    for (const ExternalDefinitionNode* curr = this->ast->head; curr != NULL; curr = curr->next) {
        sema_analyze_external_definition(this, curr);
    }
    return (SemaResult){
        .expressions_semantics = this->expressions_semantics,
        .expressions_count = this->expressions_count
    };
}