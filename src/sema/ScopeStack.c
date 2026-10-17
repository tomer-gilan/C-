//
// Created by tomer on 17-Oct-26.
//

#include "ScopeStack.h"
#include "Utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct ScopeNode ScopeNode;
struct ScopeNode {
    const Symbol** identifier_symbols; ///< array of @c Symbol* , keyed by identifier id, @c utils_xcalloc allocated
    int identifier_symbols_count;
    ScopeNode* prev;
};
struct ScopeStack {
    ScopeNode* head; ///< the file scope
    ScopeNode* tail; ///< the current scope
};

/**
 * creates a node and sets its @c prev to @b previous
 * @attention ownership of previous is transferred to the new node that's returned.
 */
static ScopeNode* scope_node_create(ScopeNode* const previous) {
    assert(previous != NULL && "no node is created from a NULL previous except for the file scope, and it must be created via scope_node_create_empty");

    ScopeNode* to_return = utils_xmalloc(sizeof (ScopeNode));
    *to_return = (ScopeNode){
        .identifier_symbols = utils_xcalloc(sizeof (Symbol*), previous->identifier_symbols_count),
        .identifier_symbols_count = previous->identifier_symbols_count,
        .prev = previous
    };
    memcpy(to_return->identifier_symbols, previous->identifier_symbols, sizeof(Symbol*) * previous->identifier_symbols_count);
    return to_return;
}
static ScopeNode* scope_node_create_empty(const int identifier_count) {
    assert(identifier_count >= 0);

    ScopeNode* to_return = utils_xmalloc(sizeof (ScopeNode));
    *to_return = (ScopeNode){
        .identifier_symbols = utils_xcalloc(sizeof (Symbol*), identifier_count),
        .identifier_symbols_count = identifier_count,
        .prev = NULL
    };
    return to_return;
}
static void scope_node_destroy(ScopeNode* const this) {
    assert(this != NULL);

    if (this->prev != NULL)
        scope_node_destroy(this->prev);
    free(this->identifier_symbols);
    free(this);
}
ScopeStack* scope_stack_create(const int identifier_count) {
    assert(identifier_count >= 0);

    ScopeStack* to_return = utils_xmalloc(sizeof (ScopeStack));
    to_return->tail = to_return->head = scope_node_create_empty(identifier_count);
    return to_return;
}
void scope_stack_destroy(ScopeStack* this) {
    assert(this != NULL);

    scope_node_destroy(this->tail);
    free(this);
}

bool scope_stack_is_at_file_scope(const ScopeStack* this) {
    assert(this != NULL);

    return this->tail == this->head;
}

static ScopeNode* scope_stack_detach_tail(ScopeStack* const this) {
    assert(this != NULL);
    assert(!scope_stack_is_at_file_scope(this));

    ScopeNode* const to_return = this->tail;
    this->tail = this->tail->prev;  // detaches the tail
    to_return->prev = NULL;         // severs any connection to the tail with the rest of the list.
    return to_return;
}
void scope_stack_pop(ScopeStack* const this) {
    assert(this != NULL);

    scope_node_destroy(scope_stack_detach_tail(this));
}
void scope_stack_push(ScopeStack* const this) {
    assert(this != NULL);

    this->tail = scope_node_create(this->tail);
}

void scope_stack_set(ScopeStack* const this, const int identifier_id, const Symbol* const symbol) {
    assert(this != NULL);
    assert(0 <= identifier_id && identifier_id < this->tail->identifier_symbols_count);

    this->tail->identifier_symbols[identifier_id] = symbol;
}
static const Symbol* scope_stack_at_by_scope_node(const ScopeNode* this, const int identifier_id) {
    assert(this != NULL);
    assert(0 <= identifier_id && identifier_id < this->identifier_symbols_count);

    return this->identifier_symbols[identifier_id];
}
const Symbol* scope_stack_at(const ScopeStack* const this, const int identifier_id) {
    assert(this != NULL);
    assert(0 <= identifier_id && identifier_id < this->tail->identifier_symbols_count);

    return scope_stack_at_by_scope_node(this->tail, identifier_id);
}
bool scope_stack_was_identifier_declared_in_current_scope(const ScopeStack* const this, const int identifier_id) {
    assert(this != NULL);

    // remember, if const Symbol* is NULL, then the identifier expression was not declared (at all)
    const Symbol* const current_scope_symbol = scope_stack_at(this, identifier_id);
    if (current_scope_symbol == NULL)
        return false; // not even declared, let alone declared in this scope.
    if (scope_stack_is_at_file_scope(this))
        return true;

    const Symbol* const previous_scope_symbol = scope_stack_at_by_scope_node(this->tail->prev, identifier_id);
    return current_scope_symbol != previous_scope_symbol;
}
