//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_EXTERNAL_DEFINITION_H
#define C_COMPILER_EXTERNAL_DEFINITION_H

typedef struct Parser Parser;
#include "Declaration.h"
#include "Range.h"
#include "Statement.h"

typedef struct {
    Declarator declarator;  ///< e.g. for int main(void) {...} the declarator describes info about the "int main(void)" part.
    Declarator* parameters;
    int parameters_count;
    StatementNode* body;    ///< a @c Statement of kind @c STATEMENT_KIND_COMPOUND
} ExternalDefinitionFunction;

typedef enum {
    EXTERNAL_DEFINITION_KIND_FUNCTION,
    EXTERNAL_DEFINITION_KIND__COUNT
} ExternalDefinitionKind;
typedef struct ExternalDefinitionNode {
    Range range;
    ExternalDefinitionKind kind;
    union {
        ExternalDefinitionFunction external_function_definition;
    } as;
    struct ExternalDefinitionNode* next;
} ExternalDefinitionNode;

typedef struct {
    Range range;
    ExternalDefinitionNode* head;
} TranslationUnitNode;

TranslationUnitNode* translation_unit_node_parse(Parser* parser);

#endif //C_COMPILER_EXTERNAL_DEFINITION_H
