//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_DRIVER_H
#define C_COMPILER_DRIVER_H

#include "Context.h"

typedef struct {
    int argc;
    const char* const* argv;
    Context* context;
} Driver;

/**
 * Creates a new @c Driver .
 * @param argc the same as @c main 's
 * @param argv the same as @c main 's
 * @return a @c Driver handle
 * @retval NULL upon invalid argv (specifically invalid @c SourceFile path)
 */
Driver* driver_create(int argc, const char* const* argv);
/**
 * Destroys the @c Driver .
 * @param this handle to the @c Driver
 */
void driver_destroy(Driver* this);

#endif //C_COMPILER_DRIVER_H