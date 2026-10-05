//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_FILE_POSITION_H
#define C_COMPILER_FILE_POSITION_H

#include "FilePosition.h"
#include "Context.h"

/**
 * A @c FilePosition represents a certain place within a @c SourceText.
 * It abstracts the line and column number by counting occurrences of @c '\n' .
 */
typedef struct {
    int source_text_index;  ///< the @c SourceText index of @c Context 's @c source_text_table.
    int offset;             ///< offset (in bytes) inside the source text. Can be converted to a @c LineColumn struct.
} FilePosition;

typedef struct {
    const char* file_name;
    int line; ///< 1-based line indexing
    int column; ///< 1-based column indexing
} FileLineColumn;

/**
 * advances position according to @b c. if @b c is \n, column resets and line++.
 */
void file_line_column_advance(FileLineColumn* this, char c);
/**
 * generates a @c FileLineColumn based on the current offset of the @c SourceText
 */
FileLineColumn file_position_get_file_line_column(FilePosition this, const Context* context);
/**
 * get the @c SourceText related to @b this @c FilePosition
 * @note @b context is deliberately non-const, since the @c SourceText which is returned
 * can modify the state of @c Context.
 */
SourceText* file_position_get_source_text(FilePosition this, Context* context);
/**
 * @c const version of @c file_position_get_source_text
 */
const SourceText* file_position_get_source_text_const(FilePosition this, const Context* context);
/**
 * predicate, checks if both are equal.
 * @attention assumes both @c FilePosition have the same underling @c Context.
 */
bool file_position_equals(FilePosition first, FilePosition second);

#endif //C_COMPILER_FILE_POSITION_H