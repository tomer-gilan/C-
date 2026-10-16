//
// Created by tomer on 27-Sep-26.
//


#include "FilePosition.h"

#include <assert.h>
#include <stdbool.h>

bool file_position_equals(const FilePosition first, const FilePosition second) {
    return first.source_text_index == second.source_text_index && file_position_compare(first, second) == 0;
}
int file_position_compare(const FilePosition first, const FilePosition second) {
    assert(first.source_text_index == second.source_text_index);
    if (first.offset > second.offset)
        return 1;
    if (first.offset < second.offset)
        return -1;
    return 0; // equal
}
