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
static Width codegen_generate_expression(               Codegen* const this, const ExpressionNode*          const expression_node) {
    assert(this != NULL);
    assert(expression_node != NULL);

    switch (expression_node->kind) {
        case EXPRESSION_KIND_INT_CONSTANT: {
            const Width width = WIDTH_LONG;
            emitter_emit_mov(
                this->emitter,
                width,
                operand_make_immediate(expression_node->as.int_constant.value),
                operand_make_register(register_make(REGISTER_KIND_RAX, width)));
            return width;
        }
        case EXPRESSION_KIND_UNARY: {
            const Width width = codegen_generate_expression(this, expression_node->as.unary.operand);
            const Operand rax_operand = operand_make_register(register_make(REGISTER_KIND_RAX, width));
            switch (expression_node->as.unary.kind) {
                case UNARY_EXPRESSION_KIND_BINARY_NEGATE:
                    emitter_emit_not(this->emitter, width, rax_operand);
                    break;
                case UNARY_EXPRESSION_KIND_LOGICAL_NOT:
                    // change flags to remember if %rax was 0:
                    emitter_emit_cmp(
                        this->emitter,
                        width,
                        operand_make_immediate(0),
                        rax_operand);
                    // zeros %rax (because 32 bit operations zero out all 64 bits):
                    emitter_emit_mov(
                        this->emitter,
                        WIDTH_LONG,
                        operand_make_immediate(0),
                        rax_operand);
                    // stores equality check in %al (which de-facto is also %rax):
                    emitter_emit_sete(this->emitter, operand_make_register(register_make(rax_operand.as.register_.kind, WIDTH_BYTE)));
                    break;
                case UNARY_EXPRESSION_KIND_NUMERIC_PLUS:
                    break; // no-op by design
                case UNARY_EXPRESSION_KIND_NUMERIC_MINUS:
                    emitter_emit_neg(this->emitter, width, rax_operand);
                    break;
                case UNARY_EXPRESSION_KIND_NONE:
                case UNARY_EXPRESSION_KIND__COUNT:
                    assert(false && "invalid program state");
                    break;
            }
            return width;
        }
        case EXPRESSION_KIND__COUNT:
        default:
            assert(false && "invalid program state");
            return WIDTH_BYTE; // trash return value
    }
}
static void codegen_generate_compound_statement(        Codegen* const this, const CompoundStatement*       const compound_statement) {
    assert(this != NULL);
    assert(compound_statement != NULL);

    emitter_indent(this->emitter);
    for (const BlockItem* curr = compound_statement->head; curr != NULL; curr = curr->next) {
        switch (curr->kind) {
            case BLOCK_ITEM_KIND_STATEMENT:
                codegen_generate_statement(this, curr->as.statement);
                break;
            case BLOCK_ITEM_KIND_DECLARATION: // TODO: implement when the implementing the relevant slices
            case BLOCK_ITEM_KIND__COUNT:
            default:
                assert(false && "invalid program state");
        }
    }
    emitter_dedent(this->emitter);
}
static void codegen_generate_statement(                 Codegen* const this, const StatementNode*           const statement_node) {
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
static void codegen_generate_function_definition_node(  Codegen* const this, const FunctionDefinitionNode*  const function_definition_node) {
    assert(this != NULL);
    assert(function_definition_node != NULL);

    const int identifier_id = function_definition_node->declarator->identifier.as.id;

    const Label label = label_make_symbol(this->context, identifier_id);
    // External Linkage:
    emitter_emit_external_linkage(this->emitter, label);
    // Entry Point:
    emitter_emit_label(this->emitter, label);
    // Body:
    codegen_generate_statement(this, function_definition_node->body);
}
static void codegen_generate_external_definition(       Codegen* const this, const ExternalDeclarationItem* const external_definition_node) {
    assert(this != NULL);
    assert(external_definition_node != NULL);

    // TODO: in the future this function will have more meat. currently, only external FUNCTION definitions are supported.
    codegen_generate_function_definition_node(this, external_definition_node->as.external_function_definition);
}
static Text* codegen_generate_translation_unit_node(    Codegen* const this, const TranslationUnitNode*     const translation_unit_node) {
    for (const ExternalDeclarationItem* curr = translation_unit_node->head; curr != NULL; curr = curr->next) {
        codegen_generate_external_definition(this, curr);
    }
    return emitter_detach_text(this->emitter);
}

Text* codegen_generate(Codegen* const this) {
    assert(this != NULL);

    return codegen_generate_translation_unit_node(this, this->ast);
}
