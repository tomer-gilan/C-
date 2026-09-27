//
// Created by tomer on 23-Sep-26.
//

#ifndef GIT_CODE_DATASTRUCTURES_H
#define GIT_CODE_DATASTRUCTURES_H

#include "Utils.h"

// -------------- List ---------------- //
typedef struct List List;

/**
 * Creates a new list
 * @return a list handle
 */
List* list_create();

/**
 * Destroys the list
 * @param this handle to the list
 */
void list_destroy(List* this);

void list_verify_existing(const List* this);

/**
 * checks if the list is empty.
 * @param this a handle to the list
 * @return true iff the list has no elements in it.
 */
bool list_is_empty(const List* this);

/**
 * Adds a new element to the list.
 * @attention ownership of @code value @endcode is transferred to the callee.
 * @param this a handle to the list
 * @param value the new element to add
 * @return the index of the added value
 */
int list_add(List* this, void* value);

/**
 * Removes the element at the given index
 * @attention ownership of return value is transferred back to the caller.
 * @param this a handle to the list
 * @param index the index of the element to be removed
 * @return the removed element.
 */
void* list_remove_at(List* this, int index);

/**
 * Destroys the element at the given index.
 * @param this a handle to the list
 * @param index the index of the element to be destroyed.
 */
void list_destroy_at(List* this, int index);

/**
 * Retrieves the size of the list.
 * @param this a handle to the list
 * @return the size of the list
 */
int list_get_size(const List* this);

/**
 * Retrieves the size of the list.
 * @param this a handle to the list
 * @param index the index of the element to be retrieved.
 * @return the size of the list
 */
void* list_at(List* this, int index);

/**
 *
 * @param this a handle to the list
 * @param element the element to find the index of
 * @param compare a function that takes two elements, and compares them strcmp-style (0 is equal, otherwise not).
 * @return true iff element exists in the list
 */
int list_index_of(const List* this, const void* element, int (*compare)(const void* first, const void* second));

/**
 *
 * @param this a handle to the list
 * @param element the element to be checked
 * @param compare a function that takes two elements, and compares them strcmp-style (0 is equal, otherwise not).
 * @return true iff element exists in the list
 */
bool list_does_contain(const List* this, const void* element, int (*compare)(const void* first, const void* second));

// -------------- Position ---------------- //
typedef struct {
    unsigned int line;
    unsigned int column;
} Position;

extern const Position POSITION_EMPTY;


/**
 * Sets the line and column of the postition to a new line
 * @param this a handle to the position
 */
void position_set_new_line(Position* this);
/**
 * advances column 'n' steps.
 * @param this a handle to the position
 * @param n how much to advance 'column'
 */
void position_advance(Position* this, unsigned int n);
/**
 * increments column
 * @param this a handle to the position
 */
void position_increment(Position* this);

#endif //GIT_CODE_DATASTRUCTURES_H