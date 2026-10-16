//
// Created by tomer on 14-Oct-26.
//

#ifndef C_COMPILER_ARENA_H
#define C_COMPILER_ARENA_H

typedef struct Arena Arena;

Arena* arena_create();
void arena_destroy(Arena* this);

/**
 * @attention aborts of run out of memory.
 * @retval NULL if @c element_size is 0
 */
void* arena_allocate(Arena* this, int allocation_size);

#endif //C_COMPILER_ARENA_H
