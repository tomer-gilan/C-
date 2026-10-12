//
// Created by tomer on 27-Sep-26.
//

#include "FilePosition.h"

bool file_position_equals(const FilePosition first, const FilePosition second) {
    return first.offset == second.offset && first.source_text_index == second.source_text_index;
}
