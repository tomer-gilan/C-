//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_EXPRESSION_H
#define C_COMPILER_EXPRESSION_H

typedef struct Parser Parser;
#include "Range.h"

typedef struct {
    int value;
} ExpressionIntConstant;

typedef enum {
    EXPRESSION_KIND_INT_CONSTANT,
    EXPRESSION_KIND__COUNT
} ExpressionKind;

typedef struct {
    Range range;
    int id; ///< the identifier that @c Sema uses to intern info about @c ExpressionNode
    ExpressionKind kind;
    union {
        ExpressionIntConstant expression_int_constant;
    } as;
} ExpressionNode;

const ExpressionNode* expression_node_parse(Parser* parser);

#endif //C_COMPILER_EXPRESSION_H
