//
// Created by tomer on 27-Sep-26.
//

#include "Location.h"

// --------- Position ------------- //
const Location LOCATION_EMPTY = {.line = 0, .column = 0};

void location_set_new_line(Location* const this) {
    this->column = 0;
    this->line++;
}
void location_advance(Location* const this, const unsigned int n) {
    for (int i = 0; i < n; ++i)
        location_increment(this); // the safest way to ensure any future invariant will hold.
}
void location_increment(Location* const this) {
    this->column++;
}