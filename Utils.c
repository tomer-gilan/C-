//
// Created by tomer on 23-Sep-26.
//

#include "Utils.h"

char* utils_allocate_and_copy_string(const char* const source) {
    char* copy = malloc(sizeof(char) * (strlen(source) + 1));
    strcpy(copy, source);
    return copy;
}
bool utils_is_alpha(const char c) {
    return  ('A' <= c && c <= 'Z') ||
            ('a' <= c && c <= 'z');
}
bool utils_is_numeric(const char c) {
    return '0' <= c && c <= '9';
}

int utils_char_to_digit(const char c) {
    return c - '0';
}