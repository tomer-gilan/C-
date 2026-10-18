//
// Written by Claude on 14-Oct-26. Updated on 15-Oct-26 for the growing (chunked) arena, and on 17-Oct-26 for the
// one-malloc-per-allocation, zeroing arena.
//

#include <stdint.h>
#include <string.h>

#include "Arena.h"
#include "Harness.h"

// The alignment malloc gives every block, so every arena allocation (_Alignof(max_align_t) on x86-64).
enum { ALIGNMENT = 16 };
// How many allocations the many-allocations test makes.
enum { MANY_ALLOCATIONS = 4096 };

static bool is_aligned(const void* const pointer) {
    return (uintptr_t)pointer % ALIGNMENT == 0;
}
static bool is_all_zero(const char* const bytes, const int size) {
    for (int i = 0; i < size; i++)
        if (bytes[i] != 0)
            return false;
    return true;
}

/** Every allocation starts aligned, whatever the size of the one before it. */
static void test_arena_allocations_are_aligned(void) {
    Arena* const arena = arena_create();
    const int sizes[] = {1, 5, 16, 17, 3, 8, 31, 1};

    for (int i = 0; i < (int)(sizeof sizes / sizeof sizes[0]); i++)
        CHECK(is_aligned(arena_malloc(arena, sizes[i])));

    arena_destroy(arena);
}
/**
 * Every allocation comes back zeroed (Sema's side tables rely on it). Fresh memory from the OS is zero anyway, so the
 * first arena dirties some blocks and is destroyed, and the second one likely gets them back from malloc. Under ASan,
 * malloc fills every new block with 0xbe, so the sanitizer run catches a missing memset for sure.
 */
static void test_arena_allocations_are_zeroed(void) {
    enum { SIZE = 64, COUNT = 8 };

    Arena* const dirty = arena_create();
    for (int i = 0; i < COUNT; i++)
        memset(arena_malloc(dirty, SIZE), 'x', SIZE);
    arena_destroy(dirty);

    Arena* const arena = arena_create();
    bool all_zero = true;
    for (int i = 0; i < COUNT; i++)
        if (!is_all_zero(arena_malloc(arena, SIZE), SIZE))
            all_zero = false;
    CHECK(all_zero);

    arena_destroy(arena);
}
/** arena_allocate_array returns element_count * element_sizeof zeroed bytes. */
static void test_arena_allocate_array_is_zeroed(void) {
    enum { COUNT = 10 };
    Arena* const arena = arena_create();

    const int* const array = arena_calloc(arena, sizeof(int), COUNT);
    CHECK(is_all_zero((const char*)array, sizeof(int) * COUNT)); // ASan reports it if the block is too small

    arena_destroy(arena);
}
/** Writing all of one allocation doesn't touch the next one. */
static void test_arena_allocations_do_not_overlap(void) {
    Arena* const arena = arena_create();

    char* const first = arena_malloc(arena, 20);
    char* const second = arena_malloc(arena, 20);
    memset(second, 'b', 20);
    memset(first, 'a', 20);

    bool second_unchanged = true;
    for (int i = 0; i < 20; i++)
        if (second[i] != 'b')
            second_unchanged = false;
    CHECK(second_unchanged);

    arena_destroy(arena);
}
/**
 * A zero-byte allocation (and a zero-element array) is allowed, and doesn't disturb the allocations after it. The
 * pointer it returns is unspecified (it may or may not be NULL) and must not be dereferenced, so it isn't checked.
 */
static void test_arena_zero_size_allocation_is_allowed(void) {
    Arena* const arena = arena_create();

    arena_malloc(arena, 0);
    arena_calloc(arena, sizeof(int), 0);
    char* const after = arena_malloc(arena, 8);
    if (CHECK(after != NULL)) {
        memset(after, 'a', 8);
        CHECK(after[7] == 'a');
    }

    arena_destroy(arena);
}
/**
 * Many allocations of mixed sizes: block i is filled with the byte i, and must still hold it after all the others are
 * written. Overlapping blocks show up as a wrong byte, and a block that's too small as an ASan report. Destroying the
 * arena must free every block (LeakSanitizer reports any it misses).
 */
static void test_arena_many_allocations(void) {
    Arena* const arena = arena_create();
    char* blocks[MANY_ALLOCATIONS];
    const int max_size = 50;

    bool all_allocated_and_aligned = true;
    for (int i = 0; i < MANY_ALLOCATIONS; i++) {
        const int size = 1 + i % max_size;
        blocks[i] = arena_malloc(arena, size);
        if (blocks[i] == NULL || !is_aligned(blocks[i]))
            all_allocated_and_aligned = false;
        else
            memset(blocks[i], (char)i, size);
    }
    CHECK(all_allocated_and_aligned);

    bool all_unchanged = true;
    for (int i = 0; all_allocated_and_aligned && i < MANY_ALLOCATIONS; i++)
        for (int j = 0; j < 1 + i % max_size; j++)
            if (blocks[i][j] != (char)i)
                all_unchanged = false;
    CHECK(all_unchanged);

    arena_destroy(arena);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);

    RUN_TEST(test_arena_allocations_are_aligned);
    RUN_TEST(test_arena_allocations_are_zeroed);
    RUN_TEST(test_arena_allocate_array_is_zeroed);
    RUN_TEST(test_arena_allocations_do_not_overlap);
    RUN_TEST(test_arena_zero_size_allocation_is_allowed);
    RUN_TEST(test_arena_many_allocations);

    return harness_finish();
}
