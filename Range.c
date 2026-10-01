//
// Created by tomer on 27-Sep-26.
//

#include <assert.h>

#include "Range.h"

StringView range_to_string_view(const Range this, const Context* const context) {
    assert(this.length >= 0 && "invalid range length (negative)");
    assert(source_text_is_index_in_bounds(
        file_position_get_source_file_const(this.position, context),
        this.position.offset + this.length)
        && "invalid range (outside of SourceText bounds)");

    const SourceText* source = list_at_const(context->source_file_table, this.position.source_file_index);
    return (StringView){
        .data = source->text + this.position.offset,
        .length = this.length
    };
}
Range range_from_file_positions(const FilePosition start, const FilePosition end) {
    assert(start.source_file_index == end.source_file_index && "the two FilePosition come from different files.");
    assert(start.offset <= end.offset && "start comes after end");
    return (Range){.position = start, .length = end.offset - start.offset};
}
void range_extend(Range* const this, const int n) {
    this->length += n;
}