//
// Created by tomer on 16-Oct-26.
//

#ifndef C_COMPILER_SEMA_H
#define C_COMPILER_SEMA_H
#include "Diagnostic.h"
#include "ExternalDefinition.h"
#include "Symbol.h"
#include "Type.h"

typedef struct Context Context;
typedef struct Sema Sema;

typedef struct {
    const Type** expressions_types;     ///< array of @c Type* , keyed by
    const Symbol** expressions_symbols; ///< array of @c Symbol* . Why not array of @c Symbol ? firstly, because it may be undefined (NULL), and secondly because by design it references the original @c Symbol in the scope stack, which is @c Arena allocated thus outlives its container.
    int expressions_count;              ///< count for BOTH arrays
} AnalysisData;                         ///< Simple struct that holds the return values of @c sema_analyze

Sema* sema_create(Context* context, const TranslationUnitNode* ast, int expression_count);
void sema_destroy(Sema* this);
AnalysisData sema_analyze(Sema* this);

bool sema_has_error(const Sema* this);
void sema_report_diagnostic(Sema* this, DiagnosticCode code, Range range);

#endif //C_COMPILER_SEMA_H
