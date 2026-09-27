//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_LOCATION_H
#define C_COMPILER_LOCATION_H

/**
 * A @c Location represents a certain place within a @c SourceFile.
 * It abstracts the line and column number by counting occurrences of @c '\n' .
 */
struct Location {
    int source_file_index;  ///< the @c SourceFile index of @c Context 's @c source_file_table.
    int offset;
};

#endif //C_COMPILER_LOCATION_H