//
// Created by tomer on 17-Oct-26.
//

#ifndef C_COMPILER_SCOPE_STACK_H
#define C_COMPILER_SCOPE_STACK_H
#include "Symbol.h"
#include <stdbool.h>

typedef struct ScopeStack ScopeStack;

/**
 * creates the stack, and sets a dummy node as the header.
 */
ScopeStack* scope_stack_create(int identifier_count);
void scope_stack_destroy(ScopeStack* this);

bool scope_stack_is_at_file_scope(const ScopeStack* this);

/**
 * @note unlike other pop functions, this pops and destroys the last node.
 * @attention asserts the stack is not at file scope
 */
void scope_stack_pop(ScopeStack* this);
void scope_stack_push(ScopeStack* this);

void scope_stack_set(ScopeStack* this, int identifier_id, const Symbol* symbol);
const Symbol* scope_stack_at(const ScopeStack* this, int identifier_id);

bool scope_stack_was_identifier_declared_in_current_scope(const ScopeStack* this, int identifier_id);

#endif //C_COMPILER_SCOPE_STACK_H
