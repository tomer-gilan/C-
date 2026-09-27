//
// Created by tomer on 27-Sep-26.
//

#ifndef GIT_CODE_LOCATION_H
#define GIT_CODE_LOCATION_H

// -------------- Position ---------------- //

typedef struct {
    const char* name;
    const char* text;
    int length;
} SourceFile;

typedef struct {
    SourceFile* file;
    int line;
    int column;
} Location;

/**
 * Sets the line and column of the location to a new line
 * @param this a handle to the position
 */
void location_set_new_line(Location* this);
/**
 * advances column 'n' steps.
 * @param this a handle to the position
 * @param n how much to advance 'column'
 */
void location_advance(Location* this, unsigned int n);
/**
 * increments column
 * @param this a handle to the position
 */
void location_increment(Location* this);

#endif //GIT_CODE_LOCATION_H