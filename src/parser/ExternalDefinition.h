//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_EXTERNAL_DEFINITION_H
#define C_COMPILER_EXTERNAL_DEFINITION_H

typedef struct Parser Parser;
#include "Range.h"
#include "Statement.h"

typedef struct {
    int name_identifier_id;
    StatementNode* body; ///< a @c Statement of kind @c STATEMENT_KIND_COMPOUND
} ExternalFunctionDefinition;

typedef enum {
    EXTERNAL_DEFINITION_KIND_FUNCTION,
    EXTERNAL_DEFINITION_KIND__COUNT
} ExternalDefinitionKind;
typedef struct ExternalDefinitionNode {
    Range range;
    ExternalDefinitionKind kind;
    union {
        ExternalFunctionDefinition external_function_definition;
    } as;
    struct ExternalDefinitionNode* next;
} ExternalDefinitionNode;

typedef struct {
    Range range;
    ExternalDefinitionNode* head;
} TranslationUnitNode;

TranslationUnitNode* translation_unit_node_parse(Parser* parser);

#endif //C_COMPILER_EXTERNAL_DEFINITION_H
