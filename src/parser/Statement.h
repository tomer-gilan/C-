//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_STATEMENT_H
#define C_COMPILER_STATEMENT_H
#include "Utils.h"

typedef struct Parser Parser;
#include "Expression.h"
#include "Range.h"

typedef struct BlockItem BlockItem;

// various kinds of statements:
typedef struct {
    const BlockItem* head;
} CompoundStatement;
typedef struct {
    const ExpressionNode* expression;
} ReturnStatement;

// the parent node that unions all the different statement kinds:
typedef enum {
    STATEMENT_KIND_RETURN,
    STATEMENT_KIND_COMPOUND,

    STATEMENT_KIND__COUNT,
} StatementKind;
typedef struct {
    Range range;
    StatementKind kind;
    union {
        CompoundStatement compound_statement;
        ReturnStatement return_statement;
    } as;
} StatementNode;

// inner structs:

typedef enum {
    BLOCK_ITEM_KIND_STATEMENT,
    BLOCK_ITEM_KIND_DECLARATION,
    BLOCK_ITEM_KIND__COUNT,
} BlockItemKind;
struct BlockItem{
    BlockItemKind kind;
    union {
        const StatementNode* statement;
        // TODO: add declarations later
    } as;
    BlockItem* next;
};

StatementNode* compound_statement_parse(Parser* parser);
StatementNode* statement_node_parse(Parser* parser);

#endif //C_COMPILER_STATEMENT_H
