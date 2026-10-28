//
// Created by tomer on 21-Oct-26.
//

#ifndef C_COMPILER_AST_SERIALIZER_H
#define C_COMPILER_AST_SERIALIZER_H
#include "ExternalDefinition.h"

typedef struct AstSerializer AstSerializer;

typedef enum {
    AST_SERIALIZATION_MODE_FLAT,
    AST_SERIALIZATION_MODE_NESTED,

    AST_SERIALIZATION_MODE__COUNT,
} AstSerializationMode;

AstSerializer* ast_serializer_create(AstSerializationMode serialization_mode);
void ast_serializer_destroy(AstSerializer* this);

void ast_serializer_serialize_expression(                   AstSerializer* this, const ExpressionNode*             expression_node, const TextView* annotations);
void ast_serializer_serialize_block_item(                   AstSerializer* this, const BlockItem*                  block_item);
void ast_serializer_serialize_compound_statement(           AstSerializer* this, const CompoundStatement*          compound_statement);
void ast_serializer_serialize_statement(                    AstSerializer* this, const StatementNode*              statement_node);
void ast_serializer_serialize_external_definition_function( AstSerializer* this, const FunctionDefinitionNode* external_definition_function);
void ast_serializer_serialize_external_definition(          AstSerializer* this, const ExternalDeclarationItem*     external_definition_node);

#endif //C_COMPILER_AST_SERIALIZER_H
