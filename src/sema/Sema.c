//
// Created by tomer on 16-Oct-26.
//

#include "Sema.h"
#include "Context.h"
#include "Type.h"
#include "ScopeStack.h"

#include <assert.h>
#include <stdlib.h>

struct Sema {
    Context* context;
    const TranslationUnitNode* ast;
    bool error; ///< error flag
    AnalysisData data;
    ScopeStack* scope_stack;
};

static void sema_intern_expression_type(Sema* const this, const ExpressionNode* const expression_node) {
    if (this->data.expressions_types[expression_node->id] != NULL)
        return;
    const Type* expression_type;
    switch (expression_node->kind) {
        case EXPRESSION_KIND_INT_CONSTANT:
            expression_type = type_get_int();
            break;
        case EXPRESSION_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
    this->data.expressions_types[expression_node->id] = expression_type;
}
static void sema_intern_identifier_expression_symbol(Sema* const this, const ExpressionNode* const identifier_expression_node) {
    // TODO: assert that identifier_expression_node is actually an identifier.
    if (this->data.expressions_symbols[identifier_expression_node->id] != NULL)
        return;
    // TODO: if is expression identifier kind, intern symbol (arena allocated). Else, do nothing.
}
static void sema_intern_expression(Sema* const this, const ExpressionNode* const expression_node) {
    assert(0 <= expression_node->id && expression_node->id < this->data.expressions_count);

    sema_intern_expression_type(this, expression_node);
    sema_intern_identifier_expression_symbol(this, expression_node);
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
        .data = {
            .expressions_types = arena_allocate_array(context->arena, sizeof(Type*), expression_count),
            .expressions_symbols = arena_allocate_array(context->arena, sizeof(Symbol*), expression_count),
            .expressions_count = expression_count
        },
        .scope_stack = scope_stack_create(list_get_size(context->identifiers_table))
    };
    return to_return;
}
void sema_destroy(Sema* const this) {
    scope_stack_destroy(this->scope_stack);
    free(this);
}

static void sema_analyze_statement(Sema* this, const StatementNode* statement_node); // forward declaration
// analyzing logic:
static void sema_analyze_expression(Sema* const this, const ExpressionNode* const expression_node) {
    if (sema_has_error(this))
        return;
    // TODO: actual sema logic, in future slices

    sema_intern_expression(this, expression_node);
}
static void sema_analyze_block_item(Sema* const this, const BlockItem* const block_item) {
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
static void sema_analyze_compound_statement(Sema* const this, const CompoundStatement compound_statement) {
    if (sema_has_error(this))
        return;
    for (const BlockItem* curr = compound_statement.head; curr != NULL; curr = curr->next) {
        sema_analyze_block_item(this, curr);
    }
}
static void sema_analyze_statement(Sema* const this, const StatementNode* const statement_node) {
    if (sema_has_error(this))
        return;
    switch (statement_node->kind) {
        case STATEMENT_KIND_RETURN:
            sema_analyze_expression(this, statement_node->as.return_statement.expression);
            break;
        case STATEMENT_KIND_COMPOUND:
            sema_analyze_compound_statement(this, statement_node->as.compound_statement);
            break;
        case STATEMENT_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void sema_analyze_external_definition_function(Sema* const this, const ExternalDefinitionFunction external_definition_function) {
    if (sema_has_error(this))
        return;
    // TODO: in the future, also check that the function return type matches the actually returned expression.

    const int identifier_id = external_definition_function.declarator.identifier.as.id;
    if (scope_stack_was_identifier_declared_in_current_scope(this->scope_stack, identifier_id)) {
        sema_report_diagnostic(this, DIAGNOSTIC_CODE_SEMA_SYMBOL_ALREADY_DEFINED, external_definition_function.declarator.identifier.range);
        return;
    }
    scope_stack_set(this->scope_stack, identifier_id, symbol_allocate(this->context->arena, &(external_definition_function.declarator)));
    // TODO: once we have identifier-expressions, intern the symbol.

    sema_analyze_statement(this, external_definition_function.body);
}
static void sema_analyze_external_definition(Sema* const this, const ExternalDefinitionNode* external_definition_node) {
    if (sema_has_error(this))
        return;
    switch (external_definition_node->kind) {
        case EXTERNAL_DEFINITION_KIND_FUNCTION:
            sema_analyze_external_definition_function(this, external_definition_node->as.external_function_definition);
            break;
        case EXTERNAL_DEFINITION_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
AnalysisData sema_analyze(Sema* const this) {
    for (const ExternalDefinitionNode* curr = this->ast->head; curr != NULL; curr = curr->next) {
        sema_analyze_external_definition(this, curr);
    }

    return this->data;
}

static void sema_set_error(Sema* this) {
    this->error = true;
}
bool sema_has_error(const Sema* const this) {
    return this->error;
}
void sema_report_diagnostic(Sema* const this, const DiagnosticCode code, const Range range) {
    assert(this != NULL);
    sema_set_error(this);
    context_report_diagnostic(this->context, code, range);
}