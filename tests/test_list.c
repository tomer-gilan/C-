//
// Written by Claude on 01-Oct-26.
//

#include <stdio.h>
#include <stdlib.h>

#include "Harness.h"
#include "List.h"

// helpers
int equals_int(const void* first, const void* second) {
    return *((int*)first) == *((int*)second) ? 0 : 1;
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
    return harness_finish();
}
