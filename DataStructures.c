//
// Created by tomer on 23-Sep-26.
//

#include "DataStructures.h"

struct List {
    int size;
    int capacity;
    void** elements;
} ;

// -------- List: private functions --------- //
bool list_should_expand(const List* this) {
    return this->size == this->capacity;
}
void list_expand(List* this) {
    this->capacity *= 2;
    this->elements = realloc(this->elements, this->capacity);
}
// maybe in the future add collapse..
// -------- END: List: private functions --------- //

List* list_create() {
    List* list = malloc(sizeof(List));
    list->size = 0;
    list->capacity = 0;
    list->elements = malloc(sizeof(void*) * 0); // intentionally allocating 0 bytes
                                                    // (which by the way, is NOT undefined behaviour)
    return list;
}
void list_destroy(List* this) {
    while (!list_is_empty(this))
        list_destroy_at(this, list_get_size(this) - 1);
}
bool list_is_empty(const List* this) {
    return this->size == 0;
}
void list_add(List* this, void* value) {
    if (list_should_expand(this))
        list_expand(this);
    this->elements[this->size++] = value;
}
void* list_remove_at(List* this, int const index) {
    void* to_remove = this->elements[index];
    for (int i = index; i < this->size - 1; ++i)
        this->elements[i] = this->elements[i + 1];
    this->size--;
    return to_remove;
}
void list_destroy_at(List* this, int const index) {
    free(list_remove_at(this, index));
}
int list_get_size(const List* this) {
    return this->size;
}