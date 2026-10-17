//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_ARENA_H
#define C_COMPILER_ARENA_H

typedef struct Arena Arena;

Arena* arena_create(void);
void arena_destroy(Arena* this);

/**
 * @attention aborts of run out of memory.
 */
void* arena_allocate(Arena* this, int allocation_size);
void* arena_allocate_array(Arena* this, int element_sizeof, int element_count);

#endif //C_COMPILER_ARENA_H
