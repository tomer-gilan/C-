//
// Created by tomer on 14-Oct-26.
//

#include "Arena.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Utils.h"

static const int ARENA_ALIGNMENT_FACTOR = 16;
static const int ARENA_NODE_SIZE = 1 << 12;

typedef struct ArenaNode{
    int size;
    char* bytes;
    int allocated; ///< number of bytes allocated
    struct ArenaNode* next;
} ArenaNode;
struct Arena {
    ArenaNode* head;
    ArenaNode* tail; ///< is never NULL, always points to the last node
};

static ArenaNode* arena_node_create(const int size) {
    ArenaNode* to_return = utils_xmalloc(sizeof (ArenaNode));
    *to_return = (ArenaNode){
        .size = size,
        .bytes = utils_xmalloc(sizeof(char) * size),
        .allocated = 0,
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
static bool arena_node_can_allocate(const ArenaNode* const this, const int element_size) {
    return this->allocated + element_size <= this->size;
}

static void arena_align(Arena* const this) {
    if (this->tail->allocated % ARENA_ALIGNMENT_FACTOR == 0)
        return;
    this->tail->allocated += ARENA_ALIGNMENT_FACTOR - this->tail->allocated % ARENA_ALIGNMENT_FACTOR;
}
static void arena_expand(Arena* const this) {
    if (this->head == NULL)
        this->tail = this->head = arena_node_create(ARENA_NODE_SIZE);
    else
        this->tail = this->tail->next = arena_node_create(ARENA_NODE_SIZE);
}

Arena* arena_create() {
    Arena* to_return = utils_xmalloc(sizeof (Arena));
    *to_return = (Arena){
        .head = NULL,
        .tail = NULL
    };
    arena_expand(to_return);
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
        return NULL;

    if (!arena_node_can_allocate(this->tail, allocation_size))
        arena_expand(this);
    if (!arena_node_can_allocate(this->tail, allocation_size))
        utils_abort_with_message("Arena failed to allocated block. Block is too large");

    void* const to_return = this->tail->bytes + this->tail->allocated;
    this->tail->allocated += allocation_size;
    arena_align(this); // to avoid mis-aligned access UB
    return to_return;
}