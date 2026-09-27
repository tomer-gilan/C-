//
// Created by tomer on 23-Sep-26.
//

#include "List.h"

#include "Diagnostic.h"

struct List {
    int size;
    int capacity;
    void** elements;
    void (*destroy_element)(void* element);
} ;

// -------- List: private functions --------- //
static bool list_is_at_full_capacity(const List* this) {
    return this->size == this->capacity;
}
static void list_expand(List* this) {
    this->capacity *= 2;
    this->elements = realloc(this->elements, this->capacity); // TODO: realloc mem check
}
// maybe in the future add collapse..
// -------- END: List: private functions --------- //
static bool list_is_index_in_bounds(const List* const list, const int index) {
    return 0 <= index && index < list_get_size(list);
}

List* list_create(void (*destroy_element)(void* element))  {
    List* list = malloc(sizeof(List));
    *list = (List){
        .size = 0,
        .capacity = 0,
        .elements = malloc(sizeof(void*) * 1),
        .destroy_element = destroy_element == NULL ? free : destroy_element
    };
    return list;
}
void list_destroy(List* this)  {
    while (!list_is_empty(this))
        list_pop_destroy(this);
}
int list_get_size(const List* this) {
    return this->size;
}
bool list_is_empty(const List* this) {
    return this->size == 0;
}
int list_push(List* this, void* value) {
    if (list_is_at_full_capacity(this))
        list_expand(this);
    this->elements[this->size++] = value;
    return this->size - 1;
}
void* list_pop(List* this) {
    if (list_is_empty(this))
        return NULL;
    return this->elements[--this->size];
}
void list_pop_destroy(List* this) {
    this->destroy_element(list_pop(this));
}
void* list_at(List* const this, const int index) {
    return this->elements[index];
}
const void* list_at_const(const List* this, const int index) {
    return this->elements[index];
}
int list_index_of(const List* this, const void* element, int (*compare)(const void* first, const void* second)) {
    for (int i = 0; i < this->size; ++i)
        if (compare(list_at_const(this, i), element) == 0)
            return i;
    return -1;
}
bool list_contains(const List* const this, const void* const element, int (*compare)(const void* first, const void* second)) {
    return list_index_of(this, element, compare) != -1;
}