//
// Created by tomer on 23-Sep-26.
//

#include "Utils.h"

char* utils_allocate_and_copy_string(const char* source) {
    char* copy = malloc(sizeof(char) * (strlen(source) + 1));
    strcpy(copy, source);
    return copy;
}