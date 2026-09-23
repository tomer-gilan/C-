//
// Created by tomer on 23-Sep-26.
//

#ifndef GIT_CODE_DATASTRUCTURES_H
#define GIT_CODE_DATASTRUCTURES_H

#include <stdlib.h>

typedef struct StringArray StringArray;

StringArray* string_array_create();
void string_array_destroy(StringArray* this);
void string_array_add(StringArray* this, const char* string);
int string_array_get_size(const StringArray* this);

#endif //GIT_CODE_DATASTRUCTURES_H