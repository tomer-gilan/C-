//
// Created by tomer on 27-Sep-26.
//

#include <assert.h>
#include <stdlib.h>

#include "FilePosition.h"

void file_line_column_advance(FileLineColumn* const this, const char c) {
    if (c == '\n') {
        this->column = 1;
        this->line++;
    }
    else
        this->column++;
}
FileLineColumn file_position_get_file_line_column(const FilePosition this, const Context* context) {
    assert(context != NULL && "Context was NULL");
    assert(context->source_text_table != NULL && "context->source_file_table was NULL");

    const SourceText* source_file = list_at_const(context->source_text_table, this.source_text_index);
    assert(this.offset <= source_file->data->length && "FilePosition's offset out of bounds (source_file->length)");

    FileLineColumn file_line_column = {.file_name = source_file->name, .line = 1, .column = 1};
    for (int i = 0; i < this.offset; ++i)
        file_line_column_advance(&file_line_column, source_file->data->data[i]);
    return file_line_column;
}
SourceText* file_position_get_source_text(const FilePosition this, Context* const context) {
    return context_source_text_at(context, this.source_text_index);
}
const SourceText* file_position_get_source_text_const(FilePosition const this, const Context* context) {
    return context_source_text_at_const(context, this.source_text_index);
}
bool file_position_equals(const FilePosition first, const FilePosition second) {
    return first.offset == second.offset && first.source_text_index == second.source_text_index;
}
