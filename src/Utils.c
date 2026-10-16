//
// Created by tomer on 23-Sep-26.
//

#include "Utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// -------------------- memory allocation -------------------- //
void* utils_xmalloc(const size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL && size != 0) // why size != 0? Because malloc can legally return NULL when size == 0
        utils_abort_with_message("xmalloc failed");
    return ptr;
}
void* utils_xrealloc(void* const ptr, const size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (new_ptr == NULL && size != 0) // why size != 0? Because realloc can legally return NULL when size == 0
        utils_abort_with_message("xrealloc failed");
    return new_ptr;
}

// --------- abortion utils ------------ //
void utils_abort_with_message(const char* const string) {
    fputs(string, stderr);
    fputs("\n", stderr);
    abort();
}

// -------------------- string utils -------------------- //
char* utils_allocate_and_copy_string(const char* const source) {
    return utils_allocate_and_copy_buffer(source, (int)strlen(source) + 1);
}
bool utils_string_list_equals(const void* const first, const void* const second) {
    return strcmp(first, second) == 0;
}
void* utils_string_list_allocate_and_copy(const void* const string) {
    return utils_allocate_and_copy_string(string);
}

// -------------------- other -------------------- //
char * utils_allocate_and_copy_buffer(const char* const buffer, const int size) {
    char* const copy = utils_xmalloc(sizeof(char) * size);
    memcpy(copy, buffer, size);
    return copy;
}
