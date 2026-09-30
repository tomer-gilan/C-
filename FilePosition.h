//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_FILE_POSITION_H
#define C_COMPILER_FILE_POSITION_H

#include <assert.h>
#include <stdlib.h>

#include "Context.h"
#include "SourceFile.h"

/**
 * A @c FilePosition represents a certain place within a @c SourceFile.
 * It abstracts the line and column number by counting occurrences of @c '\n' .
 */
typedef struct {
    int source_file_index;  ///< the @c SourceFile index of @c Context 's @c source_file_table.
    int offset;             ///< offset (in bytes) inside the source file. Can be converted to a @c LineColumn struct.
} FilePosition;

typedef struct {
    char* file_name;
    int line; ///< 1-based line indexing
    int column; ///< 1-based column indexing
} FileLineColumn;

/**
 * advances position according to c. if c is '\n', column resets and line++.
 */
void file_line_column_advance(FileLineColumn* this, char c);
/**
 * generates a @c FileLineColumn based on the current offset of the @c SourceFile
 */
FileLineColumn file_position_get_file_line_column(FilePosition this, const Context* context);
/**
 * get the @c SourceFile related to @b this @c FilePosition
 * @note @b context is deliberately non-const, since the @c SourceFile which is returned
 * can modify the state of @c Context.
 */
SourceFile* file_position_get_source_file(FilePosition this, Context* context);

#endif //C_COMPILER_FILE_POSITION_H