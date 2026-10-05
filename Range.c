//
// Created by tomer on 27-Sep-26.
//

#include <assert.h>

#include "Range.h"

TextView range_to_string_view(const Range this, const Context* const context) {
    assert(this.length >= 0 && "invalid range length (negative)");
    assert(source_text_is_index_in_bounds(
        file_position_get_source_text_const(this.position, context),
        this.position.offset + this.length - 1)
        && "invalid range (outside of SourceText bounds)");

    const SourceText* source = list_at_const(context->source_text_table, this.position.source_text_index);
    return (TextView){
        .data = source->data->data + this.position.offset,
        .length = this.length
    };
}
Range range_from_file_positions(const FilePosition start, const FilePosition end) {
    assert(start.source_text_index == end.source_text_index && "the two FilePosition come from different files.");
    assert(start.offset <= end.offset && "start comes after end");
    return (Range){.position = start, .length = end.offset - start.offset};
}
void range_extend(Range* const this, const int n) {
    this->length += n;
}
bool range_equals(const Range first, const Range second) {
    return first.length == second.length && file_position_equals(first.position, second.position);
}
