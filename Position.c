//
// Created by tomer on 27-Sep-26.
//

#include "Position.h"

#include "SourceFile.h"

void line_column_advance(FileLineColumn* const this, const char c) {
    if (c == '\n') {
        this->column = 0;
        this->line++;
    }
}
FileLineColumn position_get_file_line_column(const Position this, const Context* context) {
    assert(context != NULL && "Context was NULL");
    assert(context->source_file_table != NULL && "context->source_file_table was NULL");

    const SourceFile* source_file = list_at_const(context->source_file_table, this.source_file_index);
    assert(this.offset <= source_file->length && "Location's offset out of bounds (source_file->length)");

    FileLineColumn line_column = {.file_name = source_file->name, .line = 1, .column = 1};
    for (int i = 0; i < this.offset; ++i)
        line_column_advance(&line_column, source_file->text[i]);
    return line_column;
}
