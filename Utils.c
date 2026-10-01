//
// Created by tomer on 23-Sep-26.
//

#include "Utils.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// -------------------- memory allocation -------------------- //
void* xmalloc(const size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL)
        fprintf(stderr, "xmalloc failed.\n");
    return ptr;
}
void* xrealloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL)
        fprintf(stderr, "xrealloc failed.\n");
    return new_ptr;
}

// -------------------- string utils -------------------- //
char* utils_allocate_and_copy_string(const char* const source) {
    char* copy = xmalloc(sizeof(char) * (strlen(source) + 1));
    strcpy(copy, source);
    return copy;
}

int utils_string_list_strcmp(const void* const first, const void* const second) {
    return strcmp(first, second);
}
void* utils_string_list_allocate_and_copy(const void* const string) {
    return utils_allocate_and_copy_string(string);
}