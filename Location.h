//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_LOCATION_H
#define C_COMPILER_LOCATION_H

typedef struct {
    int source_file_index; ///< see @c Context 's @c source_file_table for more info
    int line;
    int column;
} Location;

/**
 * Sets the @c line and @c column of the @c Location to a new-line-position.
 * @param this a handle to the position
 */
void location_set_new_line(Location* this);
/**
 * advances column @b n steps.
 * @param this a handle to the position
 * @param n how much to advance @c column
 */
void location_advance(Location* this, unsigned int n);
/**
 * increments @c column
 * @param this a handle to the position
 */
void location_increment(Location* this);

#endif //C_COMPILER_LOCATION_H