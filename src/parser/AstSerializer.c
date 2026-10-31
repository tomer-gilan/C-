//
// Created by tomer on 21-Oct-26.
//

#include "AstSerializer.h"
#include "Printer.h"
#include "Utils.h"

#include <assert.h>
#include <stdlib.h>

#include "Context.h"

struct AstSerializer {
    AstSerializationMode serialization_mode;
    AstAnnotationMode annotation_mode;
    const ExpressionSemantics* expression_semantics;

    Printer* serializer;
    const Context* contex;
};

// static
static void ast_serializer_open_form(AstSerializer* const this, const TextView head) {
    assert(this != NULL);

    printer_print_text_view(this->serializer, text_view_value_of_literal("("));
    printer_print_text_view(this->serializer, head);
}
static void ast_serializer_begin_child(AstSerializer* const this) {
    assert(this != NULL);

    switch (this->serialization_mode) {
        case AST_SERIALIZATION_MODE_FLAT:
            printer_print_char(this->serializer, ' ');
            break;
        case AST_SERIALIZATION_MODE_NESTED:
            printer_indent(this->serializer);
            printer_print_new_line(this->serializer);
            break;
        case AST_SERIALIZATION_MODE__COUNT:
            assert(false && "invalid program state");
            break;
    }
}
static void ast_serializer_close_form(AstSerializer* const this) {
    assert(this != NULL);

    switch (this->serialization_mode) {
        case AST_SERIALIZATION_MODE_FLAT:
            printer_print_char(this->serializer, ')');
            break;
        case AST_SERIALIZATION_MODE_NESTED:
            printer_dedent(this->serializer);
            printer_print_new_line(this->serializer);
            break;
        case AST_SERIALIZATION_MODE__COUNT:
            assert(false && "invalid program state");
            break;
    }
}

static void ast_serializer_serialize_type(AstSerializer* const this, const Type* const type) {
    switch (type->kind) {
        case TYPE_KIND_INT:
            printer_print_text_view(this->serializer, text_view_value_of_literal("int"));
            break;
        case TYPE_KIND_CHAR:
            printer_print_text_view(this->serializer, text_view_value_of_literal("char"));
            break;
        case TYPE_KIND_VOID:
            printer_print_text_view(this->serializer, text_view_value_of_literal("void"));
            break;
        case TYPE_KIND_POINTER:
            ast_serializer_serialize_type(this, type->as.pointer.type);
            printer_print_char(this->serializer, '*');
            break;
        case TYPE_KIND_ARRAY:
            ast_serializer_serialize_type(this, type->as.array.type);
            printer_print_char(this->serializer, '[');
            printer_print_int(this->serializer, type->as.array.length);
            printer_print_char(this->serializer, ']');
            break;
        case TYPE_KIND_FUNCTION:
            ast_serializer_serialize_type(this, type->as.function.return_type);
            printer_print_char(this->serializer, '(');
            if (type->as.function.parameters_count == 0)
                ast_serializer_serialize_type(this, type_get_singleton(TYPE_KIND_VOID));
            else
                for (int i = 0; i < type->as.function.parameters_count; ++i) {
                    if (i != 0)
                        printer_print_char(this->serializer, ',');
                    ast_serializer_serialize_type(this, type->as.function.parameters_types[i]);
                }
            printer_print_char(this->serializer, ')');
            break;
        case TYPE_KIND__COUNT:
        case TYPE_KIND_NONE:
        default:
            assert(false && "invalid program state");
    }
}

static void ast_serializer_serialize_expression(               AstSerializer* this, const ExpressionNode*          const expression_node) {
    assert(this != NULL);
    assert(expression_node != NULL);

    switch (expression_node->kind) {
        case EXPRESSION_KIND_INT_CONSTANT:
            printer_print_int(this->serializer, expression_node->as.expression_int_constant.value);
            break;
        case EXPRESSION_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void ast_serializer_serialize_statement(                AstSerializer* this, const StatementNode*           const statement_node) {
    assert(this != NULL);
    assert(statement_node != NULL);

    switch (statement_node->kind) {
        case STATEMENT_KIND_RETURN:
            printer_print_text_view(this->serializer, text_view_value_of_literal("(return "));
            ast_serializer_serialize_expression(this, statement_node->as.return_statement.expression);
            printer_print_text_view(this->serializer, text_view_value_of_literal(")"));
            break;
        case STATEMENT_KIND_COMPOUND:
            printer_print_text_view(this->serializer, text_view_value_of_literal(" (block "));
            for (const BlockItem* curr = statement_node->as.compound_statement.head; curr != NULL; curr = curr->next)
                switch (curr->kind) {
                    case BLOCK_ITEM_KIND_STATEMENT:
                        ast_serializer_serialize_statement(this, curr->as.statement);
                        break;
                    case BLOCK_ITEM_KIND_DECLARATION: // TODO: add support
                    case BLOCK_ITEM_KIND__COUNT:
                    default:
                        assert(false && "invalid program state");
                }
            printer_print_text_view(this->serializer, text_view_value_of_literal(")"));
            break;
        case STATEMENT_KIND__COUNT:
            break;
        default:
            assert(false && "invalid program state");
    }

}
static void ast_serializer_serialize_function_definition_node( AstSerializer* this, const FunctionDefinitionNode*  const function_definition_node) {
    assert(this != NULL);
    assert(function_definition_node != NULL);

    // header:
    printer_print_text_view(this->serializer, text_view_value_of_literal("(function-definition "));
    // function identifier:
    printer_print_text_view(this->serializer, *(const TextView*)list_at_const(this->contex->identifiers_table, function_definition_node->declarator->identifier.as.id));
    printer_print_text_view(this->serializer, text_view_value_of_literal(":"));
    // function prototype:
    ast_serializer_serialize_type(this, function_definition_node->declarator->type);
    // parameters:
    printer_print_text_view(this->serializer, text_view_value_of_literal(" (parameters)"));
    for (const DeclaratorItem* curr = function_definition_node->parameters; curr != NULL; curr = curr->next) {
        printer_print_text_view(this->serializer, text_view_value_of_literal(" "));
        printer_print_text_view(this->serializer, *(const TextView*)list_at_const(this->contex->identifiers_table, curr->declarator->identifier.as.id));
        printer_print_text_view(this->serializer, text_view_value_of_literal(":"));
        ast_serializer_serialize_type(this, curr->declarator->type);
    }
    // body:
    ast_serializer_serialize_statement(this, function_definition_node->body);
    printer_print_text_view(this->serializer, text_view_value_of_literal(")"));
}
static void ast_serializer_serialize_translation_unit_node(    AstSerializer* this, const TranslationUnitNode*     const translation_unit_node) {
    assert(this != NULL);
    assert(translation_unit_node != NULL);

    printer_print_text_view(this->serializer, text_view_value_of_literal("(translation-unit "));
    for (const ExternalDeclarationItem* curr = translation_unit_node->head; curr != NULL; curr = curr->next) {
        switch (curr->kind) {
            case EXTERNAL_DECLARATION_ITEM_KIND_FUNCTION:
                ast_serializer_serialize_function_definition_node(this, curr->as.external_function_definition);
                break;
            case EXTERNAL_DECLARATION_ITEM_KIND_DECLARATION: // TODO: add support
            case EXTERNAL_DECLARATION_ITEM_KIND__COUNT:
                assert(false && "invalid program state");
        }
    }
    printer_print_text_view(this->serializer, text_view_value_of_literal(")"));
}

// API
/**
 *
 * @param serialization_mode flat or nested
 * @param expression_semantics if non @c NULL, the serialization is typed.
 * @return
 */
AstSerializer* ast_serializer_create(const Context* const context, const AstSerializationMode serialization_mode, const ExpressionSemantics* const expression_semantics) {
    const AstAnnotationMode annotation_mode = expression_semantics == NULL ? AST_ANNOTATION_MODE_NONE : AST_ANNOTATION_MODE_TYPED;

    AstSerializer* to_return = utils_xmalloc(sizeof (AstSerializer));
    *to_return = (AstSerializer){
        .serialization_mode = serialization_mode,
        .annotation_mode = annotation_mode,
        .expression_semantics = expression_semantics,
        .serializer = printer_create(),
        .contex = context
    };
    return to_return;
}
void ast_serializer_destroy(AstSerializer* const this) {
    assert(this != NULL);

    printer_destroy(this->serializer);
    free(this);
}

Text* ast_serializer_serialize(AstSerializer* const this, const TranslationUnitNode* const ast) {
    ast_serializer_serialize_translation_unit_node(this, ast);
    return printer_detach_text(this->serializer);
}