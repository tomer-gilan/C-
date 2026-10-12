//
// Created by tomer on 05-Oct-26.
//


#include "TextView.h"
#include "Utils.h"

#include <string.h>

TextView text_view_create(const int length, const char* const data) {
    return (TextView){.length = length, .data = data};
}
bool text_view_equals(const TextView first, const TextView second) {
    return  first.length == second.length &&
            memcmp(first.data, second.data, first.length) == 0;
}
// for list.h usage:
bool text_view_list_equals(const void* const first, const void* const second) {
    return text_view_equals(*(TextView *) first, *(TextView *) second);
}
void* text_view_list_duplicate(const void* const text_view) {
    TextView* new = utils_xmalloc(sizeof(TextView));
    *new = *(TextView*)text_view;
    return new;
}