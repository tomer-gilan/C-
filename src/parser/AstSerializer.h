//
// Created by tomer on 21-Oct-26.
//

#ifndef C_COMPILER_AST_SERIALIZER_H
#define C_COMPILER_AST_SERIALIZER_H
#include "ExternalDefinition.h"
#include "Sema.h"

typedef struct AstSerializer AstSerializer;

typedef enum {
    AST_SERIALIZATION_MODE_FLAT,
    AST_SERIALIZATION_MODE_NESTED,

    AST_SERIALIZATION_MODE__COUNT,
} AstSerializationMode;

typedef enum {
    AST_ANNOTATION_MODE_NONE,
    AST_ANNOTATION_MODE_TYPED,

    AST_ANNOTATION_MODE__COUNT,
} AstAnnotationMode;

AstSerializer* ast_serializer_create(const Context* context, AstSerializationMode serialization_mode, const ExpressionSemantics* expression_semantics);
void ast_serializer_destroy(AstSerializer* this);

Text* ast_serializer_serialize(AstSerializer* this, const TranslationUnitNode* ast);

#endif //C_COMPILER_AST_SERIALIZER_H
