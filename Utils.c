//
// Created by tomer on 23-Sep-26.
//

#include "Utils.h"

char* utils_allocate_and_copy_string(const char* const source) {
    char* copy = malloc(sizeof(char) * (strlen(source) + 1));
    strcpy(copy, source);
    return copy;
}

int utils_string_array_index_of(const char* const string, const char* const* const arr, const unsigned int arr_length) {
    for (int i = 0; i < arr_length; ++i)
        if (strcmp(arr[i], string) == 0)
            return i;
    return -1;
}

int list_string_strcmp(const void* const first, const void* const second) {
    return strcmp(first, second);
}