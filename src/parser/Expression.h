//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_EXPRESSION_H
#define C_COMPILER_EXPRESSION_H

typedef struct Parser Parser;
#include "Range.h"

typedef struct ExpressionNode ExpressionNode;

// IMPORTANT - when adding new entries, do not forget adding matching lookup value in Parser::parser_token_to_expression_kind
typedef enum {
    UNARY_EXPRESSION_KIND_BINARY_NEGATE,    // ~
    UNARY_EXPRESSION_KIND_LOGICAL_NOT,      // !
    UNARY_EXPRESSION_KIND_NUMERIC_PLUS,     // +
    UNARY_EXPRESSION_KIND_NUMERIC_MINUS,    // -
    UNARY_EXPRESSION_KIND_NONE,

    UNARY_EXPRESSION_KIND__COUNT,
} UnaryExpressionKind;
typedef struct {
    UnaryExpressionKind kind;
    const ExpressionNode* operand;
} ExpressionUnary;
typedef struct {
    int value;
} ExpressionIntConstant;

typedef enum {
    EXPRESSION_KIND_INT_CONSTANT,
    EXPRESSION_KIND_UNARY,

    EXPRESSION_KIND__COUNT
} ExpressionKind;
struct ExpressionNode {
    Range range;
    int id; ///< the identifier that @c Sema uses to intern info about @c ExpressionNode
    ExpressionKind kind;
    union {
        ExpressionIntConstant int_constant;
        ExpressionUnary unary;
    } as;
} ;

#endif //C_COMPILER_EXPRESSION_H
