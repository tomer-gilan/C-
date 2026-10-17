//
// Created by tomer on 14-Oct-26.
//

#include "Arena.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "Utils.h"

typedef struct ArenaNode{
    int size;
    char* bytes;
    struct ArenaNode* next;
} ArenaNode;
struct Arena {
    ArenaNode* head;
    ArenaNode* tail; ///< is never NULL, always points to the last node
};

static ArenaNode* arena_node_create(const int size) {
    assert(size >= 0);
    ArenaNode* to_return = utils_xmalloc(sizeof (ArenaNode));
    *to_return = (ArenaNode){
        .size = size,
        .bytes = utils_xmalloc(sizeof(char) * size),
        .next = NULL
    };
    return to_return;
}
static void arena_node_destroy(ArenaNode* const this) {
    assert(this != NULL);

    if (this->next != NULL)
        arena_node_destroy(this->next);
    free(this->bytes);
    free(this);
}

Arena* arena_create(void) {
    Arena* to_return = utils_xmalloc(sizeof (Arena));
    to_return->head = arena_node_create(0);
    to_return->tail = to_return->head;
    return to_return;
}
void arena_destroy(Arena* const this) {
    assert(this != NULL);

    arena_node_destroy(this->head); // recursively destroys all nodes in the linked list
    free(this);
}
void* arena_allocate(Arena* const this, const int allocation_size) {
    assert(this != NULL);
    assert(allocation_size >= 0);

    if (allocation_size == 0)
        return this->head->bytes; // this trick works because this->head is initialized to a dummy to allocation size 0.

    this->tail = this->tail->next = arena_node_create(allocation_size);
    memset(this->tail->bytes, 0, allocation_size);
    return this->tail->bytes;
}
void* arena_allocate_array(Arena* const this, const int element_sizeof, const int element_count) {
    assert(element_count >= 0);
    assert(element_sizeof >= 0);
    return arena_allocate(this, element_sizeof * element_count);
}