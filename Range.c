//
// Created by tomer on 27-Sep-26.
//

#include "Range.h"

#include <assert.h>

Range range_from_file_positions(const FilePosition start, const FilePosition end) {
    assert(start.source_text_index == end.source_text_index && "the two FilePosition come from different files.");
    assert(start.offset <= end.offset && "start comes after end");
    return (Range){.position = start, .length = end.offset - start.offset};
}
void range_extend(Range* const this, const int n) {
    assert(this->length + n >= 0);
    this->length += n;
}
bool range_equals(const Range first, const Range second) {
    return range_compare(first, second) == 0;
}
int range_compare(const Range first, const Range second) {
    if (!file_position_equals(first.position, second.position))
        return file_position_compare(first.position, second.position);
    // same position, so going by length:
    if (first.length > second.length)
        return 1;
    if (first.length < second.length)
        return -1;
    return 0; // equal
}
