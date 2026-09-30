//
// Created by tomer on 23-Sep-26.
//

#include "Utils.h"

char* utils_allocate_and_copy_string(const char* const source) {
    char* copy = malloc(sizeof(char) * (strlen(source) + 1));
    strcpy(copy, source);
    return copy;
}

int utils_string_list_strcmp(const void* const first, const void* const second) {
    return strcmp(first, second);
}
void* utils_string_list_allocate_and_copy(const void* const string) {
    return utils_allocate_and_copy_string(string);
}