//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_DECLARATION_H
#define C_COMPILER_DECLARATION_H
#include "Expression.h"
#include "Range.h"
#include "Token.h"
#include "Type.h"

typedef struct Parser Parser;
typedef struct DeclaratorItem DeclaratorItem;
typedef struct DeclaratorNode DeclaratorNode;

struct DeclaratorNode {
    Token base_type;        ///< the token that specified the declared base type
    Range range;            ///< i.e. for "int *x[3];" the declarator's range is "*x[3]", not "x". To retrieve only the range of "x", use identifier.range. @note function declarator range begins with the type-specifier and ends with ')', i.e. right before the functions body.
    const Type* type;       ///< i.e. int x -> x:int | char *a = NULL, b = 'z' -> a:char*, b:char
    Token identifier;       ///< @attention identifier.as.id this does NOT uniquely identify each @c Symbol, because {int x;} {int x;} are two different symbols. Use this only to retrieve the symbol's spelling through @c Context 's @c identifiers_table
};
struct DeclaratorItem {
    const DeclaratorNode* declarator;
    const ExpressionNode* initializer; ///< may be @c NULL
    DeclaratorItem* next;
};
typedef struct {
    Range range;
    const DeclaratorItem* head;
    // TODO: continue designing this, once working on relevant slice
} DeclarationNode;

#endif //C_COMPILER_DECLARATION_H
