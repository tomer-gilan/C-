//
// Created by tomer on 23-Sep-26.
//

#include "List.h"
#include "Utils.h"

#include <assert.h>
#include <stdlib.h>

struct List {
    int size;
    int capacity;
    void** elements;
    void (*destroy_element)(void* element);
} ;

// -------- List: private functions --------- //
static bool list_has_met_capacity(const List* this) {
    assert(this != NULL && "list handle was NULL");
    return this->size == this->capacity;
}
static void list_expand(List* this) {
    assert(this != NULL && "list handle was NULL");
    this->capacity *= 2;
    this->elements = utils_xrealloc(this->elements, sizeof(void*) * this->capacity);
}
// maybe in the future add collapse..
// -------- END: List: private functions --------- //
static bool list_is_index_in_bounds(const List* const this, const int index) {
    assert(this != NULL && "list handle was NULL");
    return 0 <= index && index < list_get_size(this);
}

List* list_create(void (*destroy_element)(void* element))  {
    assert(destroy_element != NULL && "Must provide valid destroy_element callback");

    List* list = utils_xmalloc(sizeof(List));
    *list = (List){
        .size = 0,
        .capacity = 1,
        .elements = utils_xmalloc(sizeof(void*) * 1),
        .destroy_element = destroy_element
    };
    return list;
}
void list_destroy(List* this)  {
    assert(this != NULL && "list handle was NULL");
    list_clear(this);
    free(this->elements);
    free(this);
}
int list_get_size(const List* this) {
    assert(this != NULL && "list handle was NULL");
    return this->size;
}
bool list_is_empty(const List* this) {
    assert(this != NULL && "list handle was NULL");
    return this->size == 0;
}
void list_clear(List* const this) {
    assert(this != NULL);
    while (!list_is_empty(this))
        list_pop_destroy(this);
}

int list_push(List* this, void* value) {
    assert(this != NULL && "list handle was NULL");
    if (list_has_met_capacity(this))
        list_expand(this);
    this->elements[this->size++] = value;
    return this->size - 1;
}
void* list_pop(List* this) {
    assert(this != NULL && "list handle was NULL");
    if (list_is_empty(this))
        return NULL;
    return this->elements[--this->size];
}
void list_pop_destroy(List* this) {
    assert(this != NULL && "list handle was NULL");
    assert(!list_is_empty(this) && "list was empty");
    this->destroy_element(list_pop(this));
}
void* list_at(List* const this, const int index) {
    assert(this != NULL && "list handle was NULL");
    assert(list_is_index_in_bounds(this, index) && "list index out of bounds");
    return this->elements[index];
}
const void* list_at_const(const List* this, const int index) {
    assert(this != NULL && "list handle was NULL");
    assert(list_is_index_in_bounds(this, index) && "list index out of bounds");
    return this->elements[index];
}
int list_index_of(const List* const this, const void* const element, bool (*equals)(const void*, const void*)) {
    assert(this != NULL && "list handle was NULL");
    assert(equals != NULL && "'equals' was NULL");
    for (int i = 0; i < this->size; ++i)
        if (equals(list_at_const(this, i), element))
            return i;
    return -1;
}
bool list_contains(const List* const this, const void* const element, bool (*equals)(const void*, const void*)) {
    assert(this != NULL && "list handle was NULL");
    assert(equals != NULL && "'equals' was NULL");

    if (element == NULL)
        return false;
    return list_index_of(this, element, equals) != -1;
}
int list_find_or_push(
    List* const this,
    const void* const element,
    bool (*equals)(const void*, const void*),
    void*(*duplicate)(const void*)) {
    assert(this != NULL && "list handle was NULL");
    assert(duplicate != NULL && "must provide a 'duplicate' function when calling 'list_find_or_push'.");
    assert(equals != NULL && "must provide an 'equals' function when calling 'list_find_or_push'.");

    int index_of_element = list_index_of(this, element, equals);
    if (index_of_element == -1)
        index_of_element = list_push(this, duplicate(element));
    return index_of_element;
}
void list_swap(List* const this, const int first_index, const int second_index) {
    assert(list_is_index_in_bounds(this, first_index));
    assert(list_is_index_in_bounds(this, second_index));
    void* tmp = this->elements[first_index];
    this->elements[first_index] = this->elements[second_index];
    this->elements[second_index] = tmp;
}

void list_sort(List* const this, int (*compare)(const void* first, const void* second), const bool ascending) {
    const int N = list_get_size(this);
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j) {
            const int compare_result = compare(list_at_const(this, i), list_at_const(this, j));
            if ((ascending && compare_result > 0) ||
                (!ascending && compare_result < 0))
                list_swap(this, i, j);
        }
}
