//
// Created by tomer on 23-Sep-26.
//

#include <assert.h>
#include <stdlib.h>

#include "List.h"

struct List {
    int size;
    int capacity;
    void** elements;
    void (*destroy_element)(void* element);
} ;

// -------- List: private functions --------- //
static bool list_is_at_full_capacity(const List* this) {
    assert(this != NULL && "list handle was NULL");
    return this->size == this->capacity;
}
static void list_expand(List* this) {
    assert(this != NULL && "list handle was NULL");
    this->capacity *= 2;
    this->elements = realloc(this->elements, sizeof(void*) * this->capacity);
    assert(this->elements != NULL && "realloc failed");
}
// maybe in the future add collapse..
// -------- END: List: private functions --------- //
static bool list_is_index_in_bounds(const List* const this, const int index) {
    assert(this != NULL && "list handle was NULL");
    return 0 <= index && index < list_get_size(this);
}

List* list_create(void (*destroy_element)(void* element))  {
    assert(destroy_element != NULL && "Must provide valid destroy_element callback");

    List* list = malloc(sizeof(List));
    assert(list != NULL && "malloc failed");
    *list = (List){
        .size = 0,
        .capacity = 1,
        .elements = malloc(sizeof(void*) * 1),
        .destroy_element = destroy_element
    };
    return list;
}
void list_destroy(List* this)  {
    assert(this != NULL && "list handle was NULL");
    while (!list_is_empty(this))
        list_pop_destroy(this);
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
int list_push(List* this, void* value) {
    assert(this != NULL && "list handle was NULL");
    if (list_is_at_full_capacity(this))
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
int list_index_of(const List* const this, const void* const element, int (*equals)(const void*, const void*)) {
    assert(this != NULL && "list handle was NULL");
    assert(equals != NULL && "'equals' was NULL");
    for (int i = 0; i < this->size; ++i)
        if (equals(list_at_const(this, i), element) == 0)
            return i;
    return -1;
}
bool list_contains(const List* const this, const void* const element, int (*equals)(const void*, const void*)) {
    assert(this != NULL && "list handle was NULL");
    assert(equals != NULL && "'equals' was NULL");
    return list_index_of(this, element, equals) != -1;
}
int list_find_or_push(List* const this, const void* const element, int (*equals)(const void*, const void*), void* (*duplicate)(const void*)) {
    assert(this != NULL && "list handle was NULL");
    assert(duplicate != NULL && "must provide a 'duplicate' function when calling 'list_find_or_push'.");
    assert(equals != NULL && "must provide an 'equals' function when calling 'list_find_or_push'.");

    int index_of_element = list_index_of(this, element, equals);
    if (index_of_element == -1)
        index_of_element = list_push(this, duplicate(element));
    return index_of_element;
}