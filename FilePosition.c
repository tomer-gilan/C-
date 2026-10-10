//
// Created by tomer on 27-Sep-26.
//

#include "FilePosition.h"

//SourceText* file_position_get_source_text(const FilePosition this, Context* const context) {
//    return context_source_text_at(context, this.source_text_index);
//}
//const SourceText* file_position_get_source_text_const(const FilePosition this, const Context* context) {
//    return context_source_text_at_const(context, this.source_text_index);
//}
bool file_position_equals(const FilePosition first, const FilePosition second) {
    return first.offset == second.offset && first.source_text_index == second.source_text_index;
}
