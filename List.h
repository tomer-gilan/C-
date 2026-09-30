//
// Created by tomer on 23-Sep-26.
//

#ifndef C_COMPILER_LIST_H
#define C_COMPILER_LIST_H

#include <assert.h>

#include "Utils.h"

typedef struct List List;

/**
 * Creates a new @c List
 * @warning Asserts @b destroy_element is NOT @c NULL. You may pass @c free as a 'default'
 * @param destroy_element a function that takes an element and destroys it.
 * @return a list handle
 */
List* list_create(void (*destroy_element)(void* element));
/**
 * Destroys the @c List .
 * @param this handle to the @c List
 */
void list_destroy(List* this);
/**
 * @param this a handle to the @c List .
 * @return the size of the @c List .
 */
int list_get_size(const List* this);
/**
 * @param this a handle to the @c List .
 * @return true iff the @c List has no elements in it.
 */
bool list_is_empty(const List* this);
/**
 * Adds a new element to the end of the @c List .
 * @warning ownership of @b value is transferred to the callee.
 * @note expands the list as needed.
 * @param this a handle to the @c List
 * @param value the new element to add
 * @return the index of @b value in the @c List
 */
int list_push(List* this, void* value);
/**
 * Removes the element from the top of the @c List .
 * @attention if @c List is empty, @c NULL is returned.
 * @warning ownership of return value is transferred to the caller.
 * @param this a handle to the @c List
 * @return the removed element.
 */
void* list_pop(List* this);
/**
 * Removes the element from the top of the @c List , and destroys it.
 * @warning the @c List cannot be empty.
 * @param this a handle to the @c List
 */
void list_pop_destroy(List* this);
/**
 * Retrieves a value of the @c List at a given @b index .
 * @param this a handle to the @c List
 * @param index the index of the element to be retrieved.
 * @return the @b index 'th member of the @c List
 */
void* list_at(List* this, int index);
/**
 * The @c const version of @c list_at
 */
const void* list_at_const(const List* this, int index);
/**
 * @param this a handle to the @c List .
 * @param element the element to find the index of.
 * @param equals a function that takes two elements, and returns 0 iff the two are equal (strcmp is one, even though
 * its usage is to compare as well).
 * @return the index of @b element in the @c List
 * @retval -1 if not found.
 */
int list_index_of(const List* this, const void* element, int (*equals)(const void* first, const void* second));
/**
 * @param this a handle to the @c List .
 * @param element the list member in question.
 * @param equals the same as in @c list_index_of , see more info there
 * @retval true if @b element is in the list.
 * @retval false otherwise.
 */
bool list_contains(const List* this, const void* element, int (*equals)(const void* first, const void* second));
/**
 * @param this a handle to the @c List .
 * @param element the list member in question.
 * @warning @b element 's ownership remains at the callee at all times. If @b element is not found in the @c List,
 * it is duplicated via @b duplicate and the copy is pushed to @c List
 * @warning assumes @b duplicate works correctly.
 * @param equals the same as in @c list_index_of , see more info there
 * @param duplicate a function that takes an @b element and returns a copy.
 * @return the index of the (newly duplicated or already in-list) element.
 */
int list_find_or_push(List* this, const void* element,
    int (*equals)(const void* first, const void* second), void* (*duplicate)(const void* to_duplicate));

#endif //C_COMPILER_LIST_H