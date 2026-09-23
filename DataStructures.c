//
// Created by tomer on 23-Sep-26.
//

#include "DataStructures.h"

#include "Utils.h"

struct StringArray{
    int size;
    char** entries;
};

StringArray* string_array_create() {
    StringArray* string_array = malloc(sizeof(StringArray));
    string_array->size = 0;
    string_array->entries = malloc(sizeof(char*) * 0);  // intentionally allocating 0 bytes
                                                            // (which by the way, is NOT undefined behaviour)
    return string_array;
}
void string_array_destroy(StringArray* this) {
    for (int i = 0; i < this->size; ++i)
        free(this->entries[i]);
    free(this);
}
void string_array_add(StringArray* this, const char* string) {
    this->entries = realloc(this->entries, sizeof(char*) * (++this->size));
    this->entries[this->size - 1] = utils_allocate_and_copy(string);
}
int string_array_get_size(const StringArray* this) {
    return this->size;
}