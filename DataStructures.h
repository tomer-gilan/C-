//
// Created by tomer on 23-Sep-26.
//

#ifndef GIT_CODE_DATASTRUCTURES_H
#define GIT_CODE_DATASTRUCTURES_H

#include "Utils.h"

typedef struct List List;
List* list_create();
void list_destroy(List* this);
bool list_is_empty(const List* this);
void list_add(List* this, void* value);
void* list_remove_at(List* this, int index);
void list_destroy_at(List* this, int index);
int list_get_size(const List* this);

#endif //GIT_CODE_DATASTRUCTURES_H