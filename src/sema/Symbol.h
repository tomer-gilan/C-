//
// Created by tomer on 17-Oct-26.
//

#ifndef C_COMPILER_SYMBOL_H
#define C_COMPILER_SYMBOL_H

#include "Declaration.h"

/**
 * @c Symbol is @c Arena allocated, but shows up in two different places:
 * 1. during @c Sema 's analysis, for every declaration, a @c Symbol is generated (pointing at the declarator),
 * and interned in @c ScopeStack by its identifier id. This is the only place were symbols are generated.
 * 2. every identifier-expression looks up the matching @c Symbol by its expression.id, then intern it
 * by its expression.id in @c expressions_symbols, so codegen can later can find the identifier-expression's
 * declarator, to get %rsp's offset, etc. Failing to find existing @c Symbol is a constraint violation
 * (usage before declaration), and the @c ScopeStack structure returns the last declared identifier (because there may
 * be several, all in different scopes).
 */
typedef struct {
    const Declarator* declarator;   ///< the place where the symbol was declared. the @c Symbol 's @c Type is accessible via this field
    //bool is_defined;                ///< functions: set to @c true once provided with functions body. variables? always @c true, because @c extern is currently out of scope for the compiler
} Symbol;

/**
 * @note notice, allocate and not create, because its @c Arena 'ed, not @c utils_xmalloc 'ed
 */
const Symbol* symbol_allocate(Arena* arena, const Declarator* declarator);

#endif //C_COMPILER_SYMBOL_H
