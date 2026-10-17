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
typedef struct Declarator Declarator;

struct Declarator {
    Range range;        ///< i.e. for "int *x[3];" the declarator's range is "*x[3]", not "x". To retrieve only the range of "x", use identifier.range
    const Type* type;   ///< i.e. int x -> x:int | char *a = NULL, b = 'z' -> a:char*, b:char
    Token identifier;   ///< @attention identifier.as.id this does NOT uniquely identify each @c Symbol, because {int x;} {int x;} are two different symbols. Use this only to retrieve the symbol's spelling through @c Context 's @c identifiers_table
};
typedef struct {
    Declarator declarator;
    const ExpressionNode* initializer;  ///< @note right now, the standard's BNF <initializer> can only be an expression, since initializer lists, etc. are out-of-scope for now.
    Declarator* next;
} InitDeclarator; ///< describes the struct: int *x = NULL, y = 3; where the first InitDeclarator would be a Declarator '*x' and initialize 'NULL', and the second a Declarator 'y' and initialize '3'
typedef struct {
    Range range;
    const InitDeclarator* head;
    // TODO: continue designing this, once working on relevant slice
} DeclarationNode;

#endif //C_COMPILER_DECLARATION_H
