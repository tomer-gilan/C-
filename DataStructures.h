//
// Created by tomer on 23-Sep-26.
//

#ifndef GIT_CODE_DATASTRUCTURES_H
#define GIT_CODE_DATASTRUCTURES_H

#include "Utils.h"

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

/**
 * checks if the list is empty.
 * @param this a handle to the list
 * @return true iff the list has no elements in it.
 */
bool list_is_empty(const List* this);

/**
 * Adds a new element to the list. OWNERSHIP IS TRANSFERRED TO THE CALLEE.
 * @param this a handle to the list
 * @param value the new element to add
 */
void list_add(List* this, void* value);

/**
 * Removes the element at the given index. OWNERSHIP IS TRANSFERRED TO THE CALLER.
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

#endif //GIT_CODE_DATASTRUCTURES_H