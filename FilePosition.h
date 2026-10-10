//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_FILE_POSITION_H
#define C_COMPILER_FILE_POSITION_H

#include "SourceText.h"

/**
 * A @c FilePosition represents a certain place within a @c SourceText.
 * It abstracts the line and column number by counting occurrences of @c '\n' .
 */
typedef struct {
    int source_text_index;  ///< the @c SourceText index of @c Context 's @c source_text_table.
    int offset; ///< offset (in bytes) inside the source text. Can be converted to a file:line:column format later.
} FilePosition;
/**
 * predicate, checks if both are equal.
 * @attention assumes both @c FilePosition have the same underling @c Context.
 */
bool file_position_equals(FilePosition first, FilePosition second);

#endif //C_COMPILER_FILE_POSITION_H