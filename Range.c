//
// Created by tomer on 27-Sep-26.
//

#include "Range.h"

StringView range_to_string_view(const Range this, const Context* const context) {
    const SourceFile* source = list_at(context->source_file_table, this.position.source_file_index);
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