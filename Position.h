//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_LOCATION_H
#define C_COMPILER_LOCATION_H

#include <assert.h>
#include <stdlib.h>

#include "Context.h"

/**
 * A @c Location represents a certain place within a @c SourceFile.
 * It abstracts the line and column number by counting occurrences of @c '\n' .
 */
typedef struct {
    int source_file_index;  ///< the @c SourceFile index of @c Context 's @c source_file_table.
    int offset;             ///< offset (in bytes) inside the source file. Can be converted to a @c LineColumn struct.
} Position;

typedef struct {
    char* file_name;
    int line;
    int column;
} FileLineColumn;

/**
 * advances position according to c. if c is '\n', column resets and line++.
 */
void line_column_advance(FileLineColumn* this, char c);
FileLineColumn position_get_file_line_column(Position this, const Context* context);

#endif //C_COMPILER_LOCATION_H