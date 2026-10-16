//
// Written by Claude on 14-Oct-26. Updated on 15-Oct-26 for the growing (chunked) arena.
//

#include <stdint.h>
#include <string.h>

#include "Arena.h"
#include "Harness.h"

// The alignment the arena promises (_Alignof(max_align_t) on x86-64).
enum { ALIGNMENT = 16 };
// Total bytes the growth tests allocate: far more than one chunk, whatever the (private) chunk size is.
enum { MANY_BYTES = 64 * 1024 };

static bool is_aligned(const void* const pointer) {
    return (uintptr_t)pointer % ALIGNMENT == 0;
}

/**
 * Allocates @b count blocks of @b size bytes and fills block i with the byte i, then checks that every block is
 * aligned and still holds its own byte. Overlapping blocks (also across chunks) show up as a wrong byte, and a block
 * that runs past its chunk's buffer as an ASan report.
 */
static void allocate_fill_and_check(const int size, const int count) {
    Arena* const arena = arena_create();
    char* blocks[MANY_BYTES / ALIGNMENT];
    if (!CHECK(count <= (int)(sizeof blocks / sizeof blocks[0])))
        return;

    bool all_allocated_and_aligned = true;
    for (int i = 0; i < count; i++) {
        blocks[i] = arena_allocate(arena, size);
        if (blocks[i] == NULL || !is_aligned(blocks[i]))
            all_allocated_and_aligned = false;
        else
            memset(blocks[i], (char)i, size);
    }
    CHECK(all_allocated_and_aligned);

    bool all_unchanged = true;
    for (int i = 0; all_allocated_and_aligned && i < count; i++)
        for (int j = 0; j < size; j++)
            if (blocks[i][j] != (char)i)
                all_unchanged = false;
    CHECK(all_unchanged);

    arena_destroy(arena); // LeakSanitizer reports it if a chunk isn't freed
}

/** Every allocation starts aligned, whatever the size of the one before it. */
static void test_arena_allocations_are_aligned(void) {
    Arena* const arena = arena_create();
    const int sizes[] = {1, 5, 16, 17, 3, 8, 31, 1};

    for (int i = 0; i < (int)(sizeof sizes / sizeof sizes[0]); i++)
        CHECK(is_aligned(arena_allocate(arena, sizes[i])));

    arena_destroy(arena);
}
/** A 1-byte allocation still takes a full alignment slot: the next one starts exactly ALIGNMENT bytes later. */
static void test_arena_small_allocation_is_padded(void) {
    Arena* const arena = arena_create();

    const char* const first = arena_allocate(arena, 1);
    const char* const second = arena_allocate(arena, 1);
    CHECK(second - first == ALIGNMENT);

    arena_destroy(arena);
}
/** Writing all of one allocation doesn't touch the next one. */
static void test_arena_allocations_do_not_overlap(void) {
    Arena* const arena = arena_create();

    char* const first = arena_allocate(arena, 20);
    char* const second = arena_allocate(arena, 20);
    memset(second, 'b', 20);
    memset(first, 'a', 20);

    bool second_unchanged = true;
    for (int i = 0; i < 20; i++)
        if (second[i] != 'b')
            second_unchanged = false;
    CHECK(second_unchanged);

    arena_destroy(arena);
}
/** A zero-byte allocation returns NULL (Arena.h). */
static void test_arena_zero_size_returns_null(void) {
    Arena* const arena = arena_create();

    CHECK(arena_allocate(arena, 0) == NULL);

    arena_destroy(arena);
}
/**
 * ALIGNMENT-sized blocks fill each chunk exactly (the chunk size is a multiple of ALIGNMENT), so this hits the
 * boundary of the capacity check: the block after the last one that fits must start a new chunk, not run past the
 * end. (A check that's too strict, < instead of <=, only wastes a slot and isn't visible from outside.)
 */
static void test_arena_grows_with_exactly_fitting_blocks(void) {
    allocate_fill_and_check(ALIGNMENT, MANY_BYTES / ALIGNMENT);
}
/**
 * 100-byte blocks (112 with padding) don't divide the chunk size, so each chunk ends with a gap too small for the next
 * block: the arena must move on to a new chunk instead of writing past the end of the current one.
 */
static void test_arena_grows_with_leftover_space(void) {
    allocate_fill_and_check(100, MANY_BYTES / 112);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);

    RUN_TEST(test_arena_allocations_are_aligned);
    RUN_TEST(test_arena_small_allocation_is_padded);
    RUN_TEST(test_arena_allocations_do_not_overlap);
    RUN_TEST(test_arena_zero_size_returns_null);
    RUN_TEST(test_arena_grows_with_exactly_fitting_blocks);
    RUN_TEST(test_arena_grows_with_leftover_space);

    return harness_finish();
}
