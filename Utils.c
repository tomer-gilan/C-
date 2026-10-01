//
// Created by tomer on 23-Sep-26.
//

#include "Utils.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// -------------------- memory allocation -------------------- //
void* utils_xmalloc(const size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL && size != 0) { // why size != 0? Because malloc can legally return NULL when size == 0
        fputs("xmalloc failed.\n", stderr);
        abort();
    }
    return ptr;
}
void* utils_xrealloc(void* const ptr, const size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL) {
        fputs("xrealloc failed.\n", stderr);
        abort();
    }
    return new_ptr;
}

// -------------------- string utils -------------------- //
char* utils_allocate_and_copy_string(const char* const source) {
    char* copy = utils_xmalloc(sizeof(char) * (strlen(source) + 1));
    strcpy(copy, source);
    return copy;
}

int utils_string_list_strcmp(const void* const first, const void* const second) {
    return strcmp(first, second);
}
void* utils_string_list_allocate_and_copy(const void* const string) {
    return utils_allocate_and_copy_string(string);
}