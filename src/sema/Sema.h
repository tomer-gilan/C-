//
// Created by tomer on 16-Oct-26.
//

#ifndef C_COMPILER_SEMA_H
#define C_COMPILER_SEMA_H
#include "ExternalDefinition.h"
#include "Symbol.h"
#include "Type.h"

typedef struct Context Context;
typedef struct Sema Sema;

typedef struct {
    const Type* type;                   ///< each @c expression have a value, thus also a @c Type*.
    const Symbol* identifier_symbol;    ///< for identifier-expression: the @c Symbol, or @c NULL if not defined. for any other expression type: always @c NULL.
} ExpressionSemantics;                  ///< all the semantic data @c Sema gathered about an @c ExpressionNode*
typedef struct {
    const ExpressionSemantics* expressions_semantics; ///< keyed by @c expression.id
    int expressions_count;
} SemaResult; ///< Simple struct that holds the return values of @c sema_analyze

Sema* sema_create(Context* context, const TranslationUnitNode* ast, int expression_count);
void sema_destroy(Sema* this);

SemaResult sema_analyze(Sema* this);

#endif //C_COMPILER_SEMA_H
