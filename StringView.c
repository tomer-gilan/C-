//
// Created by tomer on 28-Sep-26.
//

#include "StringView.h"

StringView string_view_from_string(const char* string) {
    return (StringView){.data = string, .length = (int)strlen(string)};
}
bool string_view_equals(const StringView first, const StringView second) {
    return  first.length == second.length &&
            memcmp(first.data, second.data, first.length) == 0;
}

// for list.h usage:
int string_view_list_equals(const void* const first, const void* const second) {
    return string_view_equals(*(StringView*)first, *(StringView*)second) ? 0 : 1;
}
void* string_view_list_duplicate(const void* const string_view) {
    StringView* new = malloc(sizeof(StringView));
    *new = *(StringView*)string_view;
    return new;
}

