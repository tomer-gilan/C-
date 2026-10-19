//
// Created by tomer on 19-Oct-26.
//

#include "Codegen.h"

#include <assert.h>
#include <stdlib.h>

#include "Context.h"

Codegen* codegen_create(Context* const context, const TranslationUnitNode* const ast, const SemaResult sema_result) {
    assert(context != NULL);
    assert(ast != NULL);

    Codegen* to_return = utils_xmalloc(sizeof (Codegen));
    *to_return = (Codegen){
        .context = context,
        .ast = ast,
        .sema_result = sema_result,
        .emitter = emitter_create(),
    };
    return to_return;
}
void codegen_destroy(Codegen* const this) {
    assert(this != NULL);

    emitter_destroy(this->emitter);
    free(this);
}

// Code Generation:

static Width codegen_width_by_type(const Type* type) {
    switch (type->kind) {
        case TYPE_KIND_INT:
            return WIDTH_LONG;
            break;
        case TYPE_KIND_CHAR:
            return WIDTH_BYTE;
            break;
        // I might be wrong but these types do not have a width:
        case TYPE_KIND_FUNCTION:
        case TYPE_KIND_POINTER:
        case TYPE_KIND_ARRAY:
        case TYPE_KIND_VOID:
        default:
            assert(false && "invalid program state");
    }
}

static void codegen_generate_statement(Codegen* this, const StatementNode* statement_node);

/**
 * generates code that leads the expression to be saved in %rax.
 */
static Width codegen_generate_expression(                    Codegen* const this, const ExpressionNode*              const expression_node) {
    assert(this != NULL);
    assert(expression_node != NULL);

    const Width rax_width = codegen_width_by_type(this->sema_result.expressions_semantics[expression_node->id].type);
    switch (expression_node->kind) {
        case EXPRESSION_KIND_INT_CONSTANT:
            emitter_emit_mov(
                this->emitter,
                rax_width,
                operand_make_immediate(expression_node->as.expression_int_constant.value),
                operand_make_register(register_make(REGISTER_KIND_RAX, rax_width)));
            break;
        case EXPRESSION_KIND__COUNT:
        default:
            assert(false && "invalid program state");
            break;
    }
    return rax_width;
}
static void codegen_generate_block_item(                    Codegen* const this, const BlockItem*                   const block_item) {
    assert(this != NULL);
    assert(block_item != NULL);

    switch (block_item->kind) {
        case BLOCK_ITEM_KIND_STATEMENT:
            codegen_generate_statement(this, block_item->as.statement);
            break;
        case BLOCK_ITEM_KIND_DECLARATION: // TODO: implement when the implementing the relevant slices
        case BLOCK_ITEM_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void codegen_generate_compound_statement(            Codegen* const this, const CompoundStatement*           const compound_statement) {
    assert(this != NULL);
    assert(compound_statement != NULL);

    emitter_indent(this->emitter);
    for (const BlockItem* curr = compound_statement->head; curr != NULL; curr = curr->next) {
        codegen_generate_block_item(this, curr);
    }
    emitter_dedent(this->emitter);
}
static void codegen_generate_statement(                     Codegen* const this, const StatementNode*               const statement_node) {
    assert(this != NULL);
    assert(statement_node != NULL);

    switch (statement_node->kind) {
        case STATEMENT_KIND_RETURN:
            codegen_generate_expression(this, statement_node->as.return_statement.expression);  // moves return value to %rax
            emitter_emit_ret(this->emitter);                                                    // returns
            break;
        case STATEMENT_KIND_COMPOUND:
            codegen_generate_compound_statement(this, &(statement_node->as.compound_statement));
            break;
        case STATEMENT_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void codegen_generate_external_definition_function(  Codegen* const this, const ExternalDefinitionFunction*  const external_definition_function) {
    assert(this != NULL);
    assert(external_definition_function != NULL);

    // the label:
    {
        const int identifier_id = external_definition_function->declarator.identifier.as.id;
        const Label label = label_make_symbol(this->context, identifier_id);
        emitter_emit_label(this->emitter, label);
    }

    // body:
    codegen_generate_statement(this, external_definition_function->body);
}
static void codegen_generate_external_definition(           Codegen* const this, const ExternalDefinitionNode*      const external_definition_node) {
    assert(this != NULL);
    assert(external_definition_node != NULL);

    // TODO: in the future this function will have more meat. currently, only external FUNCTION definitions are supported.
    codegen_generate_external_definition_function(this, &(external_definition_node->as.external_function_definition));
}
Text* codegen_generate(Codegen* const this) {
    assert(this != NULL);

    for (const ExternalDefinitionNode* curr = this->ast->head; curr != NULL; curr = curr->next) {
        codegen_generate_external_definition(this, curr);
    }
    return emitter_detach_text(this->emitter);
}
