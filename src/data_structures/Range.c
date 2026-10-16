//
// Created by tomer on 27-Sep-26.
//


#include "Range.h"

#include <assert.h>
#include <limits.h>

const Range RANGE_EMPTY = {.position = {.source_text_index = 0, .offset = 0}, .length = 0};

static int range_get_end_offset(const Range range) {
    return range.position.offset + range.length;
}
static Range range_union_other(const Range first, const Range second) {
    assert(first.position.source_text_index == second.position.source_text_index);
    const int source_file_index = first.position.source_text_index;
    const int min_offset = first.position.offset < second.position.offset ?
        first.position.offset : second.position.offset;
    const FilePosition union_position = {
        .source_text_index = source_file_index,
        .offset = min_offset
    };

    const int max_end_offset = range_get_end_offset(first) > range_get_end_offset(second) ?
        range_get_end_offset(first) : range_get_end_offset(second);
    const int union_length = max_end_offset - min_offset;

    return (Range){
        .position = union_position,
        .length = union_length
    };
}
Range range_union_array(const Range* const ranges, const int count) {
    assert(count > 0);

    Range to_return = ranges[0];
    for (int i = 1; i < count; ++i)
        to_return = range_union_other(to_return, ranges[i]);
    return to_return;
}

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
