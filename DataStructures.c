//
// Created by tomer on 23-Sep-26.
//

#include "DataStructures.h"

#include "Diagnostic.h"

struct List {
    int size;
    int capacity;
    void** elements;
} ;

// -------- List: private functions --------- //
static bool list_should_expand(const List* this) {
    return this->size == this->capacity;
}
static void list_expand(List* this) {
    this->capacity *= 2;
    this->elements = realloc(this->elements, this->capacity);
}
// maybe in the future add collapse..
// -------- END: List: private functions --------- //
static bool list_is_index_in_bounds(const List* const list, const int index) {
    return 0 <= index && index < list_get_size(list);
}

List* list_create() {
    List* list = malloc(sizeof(List));
    list->size = 0;
    list->capacity = 0;
    list->elements = malloc(sizeof(void*) * 0); // intentionally allocating 0 bytes
                                                    // (which by the way, is NOT undefined behaviour)
    return list;
}
void list_destroy(List* const this) {
    while (!list_is_empty(this))
        list_destroy_at(this, list_get_size(this) - 1);
}
void list_verify_existing(const List* this) {
    if (this == NULL) {
        diagnostic_issue(DIAGNOSTIC_CODE_UNINITIALIZED_TABLE, {.line = 0, .column = 0});
        exit(1);
    }
}
bool list_is_empty(const List* const this) {
    list_verify_existing(this);
    return this->size == 0;
}
int list_add(List* const this, void* value) {
    if (list_should_expand(this))
        list_expand(this);
    this->elements[this->size++] = value;
    return this->size - 1;
}
void* list_remove_at(List* const this, int const index) {
    list_verify_existing(this);
    void* to_remove = this->elements[index];
    for (int i = index; i < this->size - 1; ++i)
        this->elements[i] = this->elements[i + 1];
    this->size--;
    return to_remove;
}
void list_destroy_at(List* const this, int const index) {
    free(list_remove_at(this, index));
}
int list_get_size(const List* this) {
    list_verify_existing(this);
    return this->size;
}
void* list_at(List* const this, const int index) {
    list_verify_existing(this);
    if (!list_is_index_in_bounds(this, index)) {
        diagnostic_issue_int(DIAGNOSTIC_CODE_INDEX_OUT_OF_BOUNDS, {.line = 0, .column = 0}, index);
        return NULL;
    }
    return this->elements[index];
}
int list_index_of(const List* const this, const void* const element, int (*compare)(const void* first, const void* second)) {
    list_verify_existing(this);
    for (int i = 0; i < this->size; ++i)
        if (compare(this->elements[i], element) == 0)
            return i;
    return -1;
}
bool list_does_contain(const List* const this, const void* const element, int (*compare)(const void* first, const void* second)) {
    return list_index_of(this, element, compare) != -1;
}

// --------- Position ------------- //
const Position POSITION_EMPTY = {.line = 0, .column = 0};

void position_set_new_line(Position* const this) {
    this->column = 0;
    this->line++;
}
void position_advance(Position* const this, const unsigned int n) {
    for (int i = 0; i < n; ++i)
        position_increment(this); // the safest way to ensure any future invariant will hold.
}
void position_increment(Position* const this) {
    this->column++;
}