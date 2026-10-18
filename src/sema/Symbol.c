//
// Created by tomer on 17-Oct-26.
//

#include "Symbol.h"

const Symbol* symbol_allocate(Arena* const arena, const Declarator* const declarator) {
    Symbol* const to_return = arena_malloc(arena, sizeof (Symbol));
    *to_return = (Symbol){
        .declarator = declarator
    };
    return to_return;
}
