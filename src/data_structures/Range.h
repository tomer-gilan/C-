//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_RANGE_H
#define C_COMPILER_RANGE_H

#include "FilePosition.h"

/**
 * a @c Range is similar in nature to physics vector. It has a start (@c FilePosition) and a @c length .
 * As of today is it mostly used by @c Token to represent its exact spelling.
 */
typedef struct {
    FilePosition position;
    int length;
} Range;

/**
 * @attention asserts @b count is strictly positive.
 * @return the smallest @c Range which includes all @b ranges
 */
Range range_union_array(const Range* ranges, int count);
/**
 * the variadic version of @c range_union_array .
 */
#define range_union(...) range_union_array((const Range[]){__VA_ARGS__}, (int)(sizeof ((const Range[]){__VA_ARGS__}) / sizeof (Range)))

extern const Range RANGE_EMPTY;

/**
 * generates a @c Range based on two @c FilePosition.
 * @warning asserts both are sourced at the same file (since it's meaningless to talk about a range starting at one
 * file and ending in another), and that start does not come after end (can be the same).
 */
Range range_from_file_positions(FilePosition start, FilePosition end);
/**
 * @note @b n can be negative, in which case the range shrinks.
 * @attention asserts that adding @b n does not cause a negative @c Range @c length.
 */
void range_extend(Range* this, int n);
/**
 * predicate, determining if both are equal.
 * @attention assumes both have the same underling @c Context
 */
bool range_equals(Range first, Range second);
int range_compare(Range first, Range second);

#endif //C_COMPILER_RANGE_H