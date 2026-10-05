//
// Created by tomer on 05-Oct-26.
//

#include <string.h>

#include "TextView.h"
#include "Utils.h"

bool text_view_equals(const TextView first, const TextView second) {
    return  first.length == second.length &&
            memcmp(first.data, second.data, first.length) == 0;
}
// for list.h usage:
int text_view_list_compare(const void* const first, const void* const second) {
    return text_view_equals(*(TextView*)first, *(TextView*)second) ? 0 : 1;
}
void* text_view_list_duplicate(const void* const string_view) {
    TextView* new = utils_xmalloc(sizeof(TextView));
    *new = *(TextView*)string_view;
    return new;
}