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
    Range range;                        ///< from return type specifier to }
    const DeclaratorNode* declarator;   ///< e.g. "main(void)"
    const DeclaratorItem* parameters;   ///< e.g. list of parameter declarations.
    const StatementNode* body;                ///< a @c Statement of kind @c STATEMENT_KIND_COMPOUND
} FunctionDefinitionNode;

typedef enum {
    EXTERNAL_DECLARATION_ITEM_KIND_FUNCTION,
    EXTERNAL_DECLARATION_ITEM_KIND_DECLARATION,

    EXTERNAL_DECLARATION_ITEM_KIND__COUNT
} ExternalDeclarationItemKind;
typedef struct ExternalDeclarationItem {
    ExternalDeclarationItemKind kind;
    union {
        const FunctionDefinitionNode* external_function_definition;
    } as;
    struct ExternalDeclarationItem* next;
} ExternalDeclarationItem;

typedef struct {
    Range range;
    const ExternalDeclarationItem* head;
} TranslationUnitNode;

const TranslationUnitNode* translation_unit_node_parse(Parser* parser);

#endif //C_COMPILER_EXTERNAL_DEFINITION_H
