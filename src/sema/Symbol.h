//
// Created by tomer on 17-Oct-26.
//

#ifndef C_COMPILER_SYMBOL_H
#define C_COMPILER_SYMBOL_H
#include <stdbool.h>

#include "../parser/Declaration.h"
#include "../parser/Type.h"

typedef struct {
    int id;                         ///< the symbol-unique-identifier
    const Declarator* declarator;   ///< the place where the symbol was declared. the @c Symbol 's @c Type is accessible via this field
    bool is_defined;                ///< functions: set to @c true once provided with functions body. variables? always @c true, because @c extern is currently out of scope for the compiler
} Symbol;

#endif //C_COMPILER_SYMBOL_H
