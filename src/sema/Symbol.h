//
// Created by tomer on 17-Oct-26.
//

#ifndef C_COMPILER_SYMBOL_H
#define C_COMPILER_SYMBOL_H

#include "Arena.h"
#include "Declaration.h"

/**
 * @b Two @b sentence @b explanation:
 *
 * A @c Declarator creates a @c Symbol* and puts it in the @c ScopeStack.
 *
 * An @b identifier-expression looks up the @c Symbol* in the @c ScopeStack , and puts it in @c expressions_symbols.
 *
 *
 * @b In @b short:
 *
 * Problem:
 *
 * every identifier may appear more than once. So @c identifier.id alone cannot uniquely identify each @c Symbol.
 *
 * Solution:
 * 1. during @c Sema 's analysis, the @c ScopeStack tracks each @c identifier.id, updating the @c Symbol* when it's
 * shadowed by another. The size of the stack is identifier_count * scope redundancy
 * 2. meanwhile, an @c expressions_symbols table is populated with @c Symbol* from the @c ScopeStack
 * (for @c Codegen to use later).
 * It is a flat 1d array, keyed by each identifier-expression's id. The point is to have O(1) access from any
 * identifier-expression to its @c Declarator , for @c Codegen to use.
 * The size is: expression_count (though only identifier-expressions are populated, the rest are NULL)
 *
 * @b More @b info:
 *
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
    const DeclaratorNode* declarator;   ///< the place where the symbol was declared. the @c Symbol 's @c Type is accessible via this field
    //bool is_defined;                ///< functions: set to @c true once provided with functions body. variables? always @c true, because @c extern is currently out of scope for the compiler
} Symbol;

/**
 * @note notice, allocate and not create, because its @c Arena 'ed, not @c utils_xmalloc 'ed
 */
const Symbol* symbol_allocate(Arena* arena, const DeclaratorNode* declarator);

#endif //C_COMPILER_SYMBOL_H
