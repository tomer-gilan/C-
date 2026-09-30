//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_RANGE_H
#define C_COMPILER_RANGE_H

#include "FilePosition.h"
#include "StringView.h"

/**
 * a @c Range is similar in nature to physics vector. It has a start (@c Location) and a @c length .
 * As of today is it mostly used by @c Token to represent its exact spelling.
 */
typedef struct {
    FilePosition position;
    int length;
} Range;

/**
 * returns a @c StringView based on the @c Range @b range given
 */
StringView range_to_string_view(Range this, const Context* context);
/**
 * generates a @c Range based on two @c FilePosition.
 * @warning asserts both are sourced at the same file (since it's meaningless to talk about a range starting at one
 * file and ending in another), and that start does not come after end (can be the same).
 */
Range range_from_file_positions(FilePosition start, FilePosition end);
void range_extend(Range* this, int n);

#endif //C_COMPILER_RANGE_H