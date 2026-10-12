//
// Written by Claude on 01-Oct-26.
//

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "Harness.h"
#include "List.h"

// helpers
bool equals_int(const void* first, const void* second) {
    return *(int *) first == *(int *) second;
}
void* duplicate_int(const void* first) {
    int* to_return = malloc(sizeof(int));
    *to_return = *((int*)first);
    return to_return;
}
// end helpers

static void test_list_create_is_empty(void) {
    List* const list = list_create(free);

    CHECK(list_is_empty(list));
    CHECK(list_get_size(list) == 0);

    list_destroy(list);
}
static void test_list_pop(void) {
    List* const list = list_create(free);

    CHECK(list_pop(list) == NULL);

    const int n = 3;
    list_find_or_push(list, &n, equals_int, duplicate_int);
    CHECK(!list_is_empty(list));

    list_destroy(list);
}
static void test_list_push_pop_indexes(void) {
    List* const list = list_create(free);
    int* p1 = malloc(sizeof(int));
    int* p2 = malloc(sizeof(int));
    int* p3 = malloc(sizeof(int));
    int* p4 = malloc(sizeof(int));
    int* p5 = malloc(sizeof(int));
    CHECK(list_push(list, p1) == 0);
    CHECK(list_push(list, p2) == 1);
    CHECK(list_push(list, p3) == 2);
    CHECK(list_push(list, p4) == 3);
    CHECK(list_push(list, p5) == 4);

    CHECK(list_pop(list) == p5);
    CHECK(list_pop(list) == p4);
    CHECK(list_pop(list) == p3);
    CHECK(list_pop(list) == p2);
    CHECK(list_pop(list) == p1);
    free(p1);
    free(p2);
    free(p3);
    free(p4);
    free(p5);

    list_destroy(list);
}
static void test_list_at(void) {
    List* const list = list_create(free);
    int* p1 = malloc(sizeof(int));
    int* p2 = malloc(sizeof(int));
    int* p3 = malloc(sizeof(int));
    int* p4 = malloc(sizeof(int));
    int* p5 = malloc(sizeof(int));
    list_push(list, p1);
    list_push(list, p2);
    list_push(list, p3);
    list_push(list, p4);
    list_push(list, p5);
    CHECK(list_at(list, 0) == p1);
    CHECK(list_at(list, 1) == p2);
    CHECK(list_at(list, 2) == p3);
    CHECK(list_at(list, 3) == p4);
    CHECK(list_at(list, 4) == p5);
    list_destroy(list);
}
static void test_list_at_const(void) {
    List* const list = list_create(free);
    int* p1 = malloc(sizeof(int));
    int* p2 = malloc(sizeof(int));
    int* p3 = malloc(sizeof(int));
    int* p4 = malloc(sizeof(int));
    int* p5 = malloc(sizeof(int));
    list_push(list, p1);
    list_push(list, p2);
    list_push(list, p3);
    list_push(list, p4);
    list_push(list, p5);
    CHECK(list_at_const(list, 0) == p1);
    CHECK(list_at_const(list, 1) == p2);
    CHECK(list_at_const(list, 2) == p3);
    CHECK(list_at_const(list, 3) == p4);
    CHECK(list_at_const(list, 4) == p5);
    list_destroy(list);
}
static void test_list_index_of(void) {
    List* const list = list_create(free);

    int* p1 = malloc(sizeof (int));
    int* p2 = malloc(sizeof (int));
    int* p3 = malloc(sizeof (int));
    int* p4 = malloc(sizeof (int));
    int* p5 = malloc(sizeof (int));

    *p1 = 5;
    *p2 = 1;
    *p3 = 2;
    *p4 = 4;
    *p5 = 3;

    list_push(list, p1);
    list_push(list, p2);
    list_push(list, p3);
    list_push(list, p4);
    list_push(list, p5);

    const int n1 = 1, n2 = 2, n3 = 3, n4 = 4, n5 = 5;
    CHECK(list_index_of(list, &n5, equals_int) == 0);
    CHECK(list_index_of(list, &n1, equals_int) == 1);
    CHECK(list_index_of(list, &n2, equals_int) == 2);
    CHECK(list_index_of(list, &n4, equals_int) == 3);
    CHECK(list_index_of(list, &n3, equals_int) == 4);

    const int n6 = 6;
    CHECK(list_index_of(list, &n6, equals_int) == -1);

    list_destroy(list);
}
static void test_list_contains(void) {
    List* const list = list_create(free);

    int* p1 = malloc(sizeof (int));
    int* p2 = malloc(sizeof (int));
    int* p3 = malloc(sizeof (int));
    int* p4 = malloc(sizeof (int));
    int* p5 = malloc(sizeof (int));

    const int n1 = 5, n2 = 1, n3 = 2, n4 = 4, n5 = 3;
    *p1 = 5;
    *p2 = 1;
    *p3 = 2;
    *p4 = 4;
    *p5 = 3;

    CHECK(!list_contains(list, p1, equals_int));
    CHECK(!list_contains(list, p2, equals_int));
    CHECK(!list_contains(list, p3, equals_int));
    CHECK(!list_contains(list, p4, equals_int));
    CHECK(!list_contains(list, p5, equals_int));

    CHECK(!list_contains(list, &n1, equals_int));
    CHECK(!list_contains(list, &n2, equals_int));
    CHECK(!list_contains(list, &n3, equals_int));
    CHECK(!list_contains(list, &n4, equals_int));
    CHECK(!list_contains(list, &n5, equals_int));
    list_push(list, p1);
    CHECK(list_contains(list, p1, equals_int));
    CHECK(!list_contains(list, p2, equals_int));
    CHECK(!list_contains(list, p3, equals_int));
    CHECK(!list_contains(list, p4, equals_int));
    CHECK(!list_contains(list, p5, equals_int));

    CHECK(list_contains(list, &n1, equals_int));
    CHECK(!list_contains(list, &n2, equals_int));
    CHECK(!list_contains(list, &n3, equals_int));
    CHECK(!list_contains(list, &n4, equals_int));
    CHECK(!list_contains(list, &n5, equals_int));
    list_push(list, p2);
    CHECK(list_contains(list, p1, equals_int));
    CHECK(list_contains(list, p2, equals_int));
    CHECK(!list_contains(list, p3, equals_int));
    CHECK(!list_contains(list, p4, equals_int));
    CHECK(!list_contains(list, p5, equals_int));

    CHECK(list_contains(list, &n1, equals_int));
    CHECK(list_contains(list, &n2, equals_int));
    CHECK(!list_contains(list, &n3, equals_int));
    CHECK(!list_contains(list, &n4, equals_int));
    CHECK(!list_contains(list, &n5, equals_int));
    list_push(list, p3);
    CHECK(list_contains(list, p1, equals_int));
    CHECK(list_contains(list, p2, equals_int));
    CHECK(list_contains(list, p3, equals_int));
    CHECK(!list_contains(list, p4, equals_int));
    CHECK(!list_contains(list, p5, equals_int));

    CHECK(list_contains(list, &n1, equals_int));
    CHECK(list_contains(list, &n2, equals_int));
    CHECK(list_contains(list, &n3, equals_int));
    CHECK(!list_contains(list, &n4, equals_int));
    CHECK(!list_contains(list, &n5, equals_int));
    list_push(list, p4);
    CHECK(list_contains(list, p1, equals_int));
    CHECK(list_contains(list, p2, equals_int));
    CHECK(list_contains(list, p3, equals_int));
    CHECK(list_contains(list, p4, equals_int));
    CHECK(!list_contains(list, p5, equals_int));

    CHECK(list_contains(list, &n1, equals_int));
    CHECK(list_contains(list, &n2, equals_int));
    CHECK(list_contains(list, &n3, equals_int));
    CHECK(list_contains(list, &n4, equals_int));
    CHECK(!list_contains(list, &n5, equals_int));
    list_push(list, p5);
    CHECK(list_contains(list, p1, equals_int));
    CHECK(list_contains(list, p2, equals_int));
    CHECK(list_contains(list, p3, equals_int));
    CHECK(list_contains(list, p4, equals_int));
    CHECK(list_contains(list, p5, equals_int));

    CHECK(list_contains(list, &n1, equals_int));
    CHECK(list_contains(list, &n2, equals_int));
    CHECK(list_contains(list, &n3, equals_int));
    CHECK(list_contains(list, &n4, equals_int));
    CHECK(list_contains(list, &n5, equals_int));

    list_destroy(list);
}
static void test_list_find_or_push(void) {
    List* const list = list_create(free);
    int* p1 = malloc(sizeof(int));
    int* p2 = malloc(sizeof(int));
    int* p3 = malloc(sizeof(int));
    int* p4 = malloc(sizeof(int));
    int* p5 = malloc(sizeof(int));
    *p1 = 5;
    *p2 = 1;
    *p3 = 2;
    *p4 = 4;
    *p5 = 3;

    CHECK(list_get_size(list) == 0);

    CHECK(list_find_or_push(list, p1, equals_int, duplicate_int) == 0);
    CHECK(list_get_size(list) == 1);

    CHECK(list_find_or_push(list, p1, equals_int, duplicate_int) == 0);
    CHECK(list_get_size(list) == 1);

    CHECK(list_find_or_push(list, p2, equals_int, duplicate_int) == 1);
    CHECK(list_get_size(list) == 2);

    CHECK(list_find_or_push(list, p3, equals_int, duplicate_int) == 2);
    CHECK(list_get_size(list) == 3);

    CHECK(list_find_or_push(list, p1, equals_int, duplicate_int) == 0);
    CHECK(list_get_size(list) == 3);

    CHECK(list_find_or_push(list, p4, equals_int, duplicate_int) == 3);
    CHECK(list_get_size(list) == 4);

    CHECK(list_find_or_push(list, p5, equals_int, duplicate_int) == 4);
    CHECK(list_get_size(list) == 5);

    CHECK(list_find_or_push(list, p3, equals_int, duplicate_int) == 2);
    CHECK(list_get_size(list) == 5);

    free(p1);
    free(p2);
    free(p3);
    free(p4);
    free(p5);
    list_destroy(list);
}

// helpers for the swap and sort tests (written by Claude on 12-Oct-26)

/** A @c compare for ints: negative, zero or positive, and safe for every int (unlike @c a-b ). */
static int compare_int(const void* const first, const void* const second) {
    const int a = *(const int *) first;
    const int b = *(const int *) second;
    return (a > b) - (a < b);
}

/** A @c compare that returns the difference, so the sign is right but the size isn't 1. Only for small values. */
static int compare_int_by_difference(const void* const first, const void* const second) {
    return *(const int *) first - *(const int *) second;
}

/** A @c compare that returns the extreme values of @c int , as @c strcmp -style functions may (any sign is valid). */
static int compare_int_extreme(const void* const first, const void* const second) {
    const int a = *(const int *) first;
    const int b = *(const int *) second;
    return a < b ? INT_MIN : (a > b ? INT_MAX : 0);
}

/**
 * @param values the ints to copy into a new list
 * @param count how many there are
 * @return a new list that owns a heap copy of each of @b values , in order
 */
static List *list_of_ints(const int* const values, const int count) {
    List* const list = list_create(free);
    for (int i = 0; i < count; ++i) {
        int* const element = malloc(sizeof(int));
        *element = values[i];
        list_push(list, element);
    }
    return list;
}

/**
 * Checks that @b list holds exactly @b expected , in order. On a mismatch, prints both.
 */
static void check_list_ints(const List* const list, const int* const expected, const int count) {
    bool matches = list_get_size(list) == count;
    for (int i = 0; matches && i < count; ++i)
        matches = *(const int *) list_at_const(list, i) == expected[i];
    if (CHECK(matches))
        return;
    printf("  list mismatch, expected:");
    for (int i = 0; i < count; ++i)
        printf(" %d", expected[i]);
    printf(", got:");
    for (int i = 0; i < list_get_size(list); ++i)
        printf(" %d", *(const int *) list_at_const(list, i));
    printf("\n");
    fflush(stdout);
}

/**
 * Swapping two different indexes exchanges those two elements and nothing else. The elements themselves are not
 * copied: the same pointers end up at the other index.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_swap_exchanges_elements(void) {
    const int values[] = {10, 20, 30, 40, 50};
    List* const list = list_of_ints(values, 5);
    const void* const second = list_at(list, 1);
    const void* const fourth = list_at(list, 3);

    list_swap(list, 1, 3);

    const int expected[] = {10, 40, 30, 20, 50};
    check_list_ints(list, expected, 5);
    CHECK(list_at(list, 1) == fourth);
    CHECK(list_at(list, 3) == second);
    list_destroy(list);
}

/**
 * The first and the last element are valid indexes for a swap (the edges of the bounds check).
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_swap_first_and_last(void) {
    const int values[] = {10, 20, 30, 40, 50};
    List* const list = list_of_ints(values, 5);

    list_swap(list, 0, 4);

    const int expected[] = {50, 20, 30, 40, 10};
    check_list_ints(list, expected, 5);
    list_destroy(list);
}

/**
 * Swapping an index with itself changes nothing (and doesn't lose the element).
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_swap_same_index(void) {
    const int values[] = {10, 20, 30};
    List* const list = list_of_ints(values, 3);

    list_swap(list, 1, 1);

    check_list_ints(list, values, 3);
    list_destroy(list);
}

/**
 * The order of the two indexes doesn't matter: swap(3, 1) is swap(1, 3). And swapping the same pair twice puts
 * everything back.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_swap_symmetric_and_involutive(void) {
    const int values[] = {10, 20, 30, 40, 50};
    List* const list = list_of_ints(values, 5);

    list_swap(list, 3, 1);
    const int swapped[] = {10, 40, 30, 20, 50};
    check_list_ints(list, swapped, 5);

    list_swap(list, 1, 3);
    check_list_ints(list, values, 5);
    list_destroy(list);
}

/**
 * The smallest list a swap can work on: two elements. The size stays the same, and the list is still usable
 * (pop returns the new last element).
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_swap_two_elements(void) {
    const int values[] = {1, 2};
    List* const list = list_of_ints(values, 2);

    list_swap(list, 0, 1);

    const int expected[] = {2, 1};
    check_list_ints(list, expected, 2);
    int* const popped = list_pop(list);
    CHECK(*popped == 1);
    free(popped);
    list_destroy(list);
}

/**
 * An empty list and a one-element list are already sorted, in both directions: no crash, nothing changes.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_empty_and_single(void) {
    List* const empty = list_create(free);
    list_sort(empty, compare_int, true);
    list_sort(empty, compare_int, false);
    CHECK(list_is_empty(empty));
    list_destroy(empty);

    const int values[] = {7};
    List* const single = list_of_ints(values, 1);
    list_sort(single, compare_int, true);
    check_list_ints(single, values, 1);
    list_sort(single, compare_int, false);
    check_list_ints(single, values, 1);
    list_destroy(single);
}

/**
 * Ascending puts the smallest first. Duplicates stay (both 1s are still there).
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_ascending(void) {
    const int values[] = {5, 1, 4, 1, 3};
    List* const list = list_of_ints(values, 5);

    list_sort(list, compare_int, true);

    const int expected[] = {1, 1, 3, 4, 5};
    check_list_ints(list, expected, 5);
    list_destroy(list);
}

/**
 * Descending puts the largest first.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_descending(void) {
    const int values[] = {5, 1, 4, 1, 3};
    List* const list = list_of_ints(values, 5);

    list_sort(list, compare_int, false);

    const int expected[] = {5, 4, 3, 1, 1};
    check_list_ints(list, expected, 5);
    list_destroy(list);
}

/**
 * The inputs a sorting algorithm is usually wrong on: already sorted, sorted the other way around, two elements, and
 * all equal. Each is sorted ascending and descending.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_already_sorted_reversed_and_equal(void) {
    const int ascending[] = {1, 2, 3, 4, 5};
    const int descending[] = {5, 4, 3, 2, 1};
    const int equal[] = {2, 2, 2, 2};
    const int two[] = {2, 1};
    const int two_ascending[] = {1, 2};

    List* list = list_of_ints(ascending, 5);
    list_sort(list, compare_int, true);
    check_list_ints(list, ascending, 5);
    list_sort(list, compare_int, false);
    check_list_ints(list, descending, 5);
    list_sort(list, compare_int, false);
    check_list_ints(list, descending, 5);
    list_sort(list, compare_int, true);
    check_list_ints(list, ascending, 5);
    list_destroy(list);

    list = list_of_ints(equal, 4);
    list_sort(list, compare_int, true);
    check_list_ints(list, equal, 4);
    list_sort(list, compare_int, false);
    check_list_ints(list, equal, 4);
    list_destroy(list);

    list = list_of_ints(two, 2);
    list_sort(list, compare_int, true);
    check_list_ints(list, two_ascending, 2);
    list_destroy(list);
}

/**
 * Negative numbers sort before positive ones, and the extremes of @c int work (INT_MIN, INT_MAX): this is what a
 * comparator written as @c a-b would get wrong, so the test uses a safe one (@c compare_int ).
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_negative_and_extreme_values(void) {
    const int values[] = {0, INT_MAX, -3, INT_MIN, 3, -1};
    List* const list = list_of_ints(values, 6);

    list_sort(list, compare_int, true);

    const int expected[] = {INT_MIN, -3, -1, 0, 3, INT_MAX};
    check_list_ints(list, expected, 6);
    list_destroy(list);
}

/**
 * Only the sign of what @c compare returns matters, not its size (like @c strcmp ): a comparator that returns the
 * difference (any positive or negative number) sorts both ways.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_uses_only_the_sign_of_compare(void) {
    const int values[] = {3, -2, 7, 0, -9};
    List* const list = list_of_ints(values, 5);

    list_sort(list, compare_int_by_difference, true);
    const int ascending[] = {-9, -2, 0, 3, 7};
    check_list_ints(list, ascending, 5);

    list_sort(list, compare_int_by_difference, false);
    const int descending[] = {7, 3, 0, -2, -9};
    check_list_ints(list, descending, 5);
    list_destroy(list);
}

/**
 * Sorting only moves elements around: every element that went in is still in the list exactly once, as the very same
 * pointer (nothing copied, lost or freed), so the list can still free them all.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_keeps_the_same_elements(void) {
    const int values[] = {5, 1, 4, 1, 3};
    List* const list = list_of_ints(values, 5);
    const void* original[5];
    for (int i = 0; i < 5; ++i)
        original[i] = list_at(list, i);

    list_sort(list, compare_int, true);

    CHECK(list_get_size(list) == 5);
    for (int i = 0; i < 5; ++i) {
        int occurrences = 0;
        for (int j = 0; j < list_get_size(list); ++j)
            if (list_at_const(list, j) == original[i])
                occurrences++;
        CHECK(occurrences == 1);
    }
    list_destroy(list);
}

/**
 * A longer list than any hand-written case (300 pseudo-random ints, from a fixed seed so the test is the same every
 * run, with many duplicates): ascending ends up in non-decreasing order, descending in non-increasing order, and the
 * sum is the same as before (nothing changed value).
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_many_elements(void) {
    enum { COUNT = 300 };
    int values[COUNT];
    long sum = 0;
    unsigned state = 12345;
    for (int i = 0; i < COUNT; ++i) {
        state = state * 1103515245u + 12345u;
        values[i] = (int) ((state >> 16) % 100) - 50;
        sum += values[i];
    }
    List* const list = list_of_ints(values, COUNT);

    list_sort(list, compare_int, true);
    bool in_order = list_get_size(list) == COUNT;
    long sum_after = 0;
    for (int i = 0; i < COUNT; ++i) {
        const int current = *(const int *) list_at_const(list, i);
        sum_after += current;
        if (i > 0 && *(const int *) list_at_const(list, i - 1) > current)
            in_order = false;
    }
    CHECK(in_order);
    CHECK(sum_after == sum);

    list_sort(list, compare_int, false);
    in_order = list_get_size(list) == COUNT;
    sum_after = 0;
    for (int i = 0; i < COUNT; ++i) {
        const int current = *(const int *) list_at_const(list, i);
        sum_after += current;
        if (i > 0 && *(const int *) list_at_const(list, i - 1) < current)
            in_order = false;
    }
    CHECK(in_order);
    CHECK(sum_after == sum);
    list_destroy(list);
}

/**
 * A comparator may return any negative or positive int, INT_MIN and INT_MAX included (like @c strcmp ). Ascending
 * is fine with that.
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_extreme_compare_results_ascending(void) {
    const int values[] = {2, 3, 1};
    List* const list = list_of_ints(values, 3);

    list_sort(list, compare_int_extreme, true);

    const int expected[] = {1, 2, 3};
    check_list_ints(list, expected, 3);
    list_destroy(list);
}

/**
 * Same comparator, descending. @c list_sort negates the result with @c -1 * compare(...) , and @c -1 * INT_MIN
 * overflows an int, which is undefined behavior; the sanitizer build stops the program there.
 * @attention FAILS (UBSan aborts the whole test executable, so run it last) until @c list_sort compares the result
 * with 0 without negating it, e.g. @c compare(b, a) for descending, or @c ascending ? r > 0 : r < 0 .
 * @note Written by Claude on 12-Oct-26, not by Tomer.
 */
static void test_list_sort_extreme_compare_results_descending(void) {
    const int values[] = {1, 2, 3};
    List* const list = list_of_ints(values, 3);

    list_sort(list, compare_int_extreme, false);

    const int expected[] = {3, 2, 1};
    check_list_ints(list, expected, 3);
    list_destroy(list);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);
    RUN_TEST(test_list_create_is_empty);
    RUN_TEST(test_list_push_pop_indexes);
    RUN_TEST(test_list_pop);
    RUN_TEST(test_list_at);
    RUN_TEST(test_list_at_const);
    RUN_TEST(test_list_index_of);
    RUN_TEST(test_list_contains);
    RUN_TEST(test_list_find_or_push);
    // swap and sort tests (written by Claude)
    RUN_TEST(test_list_swap_exchanges_elements);
    RUN_TEST(test_list_swap_first_and_last);
    RUN_TEST(test_list_swap_same_index);
    RUN_TEST(test_list_swap_symmetric_and_involutive);
    RUN_TEST(test_list_swap_two_elements);
    RUN_TEST(test_list_sort_empty_and_single);
    RUN_TEST(test_list_sort_ascending);
    RUN_TEST(test_list_sort_descending);
    RUN_TEST(test_list_sort_already_sorted_reversed_and_equal);
    RUN_TEST(test_list_sort_negative_and_extreme_values);
    RUN_TEST(test_list_sort_uses_only_the_sign_of_compare);
    RUN_TEST(test_list_sort_keeps_the_same_elements);
    RUN_TEST(test_list_sort_many_elements);
    RUN_TEST(test_list_sort_extreme_compare_results_ascending);
    RUN_TEST(test_list_sort_extreme_compare_results_descending); // last: UBSan aborts the process when it fails
    return harness_finish();
}
