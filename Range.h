//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_RANGE_H
#define C_COMPILER_RANGE_H

#include "Position.h"

/**
 * a @c Range is similar in nature to physics vector. It has a start (@c Location) and a @c length .
 * As of today is it mostly used by @c Token to represent its exact spelling.
 */
typedef struct {
    Position position;
    int length;
} Range;

#endif //C_COMPILER_RANGE_H