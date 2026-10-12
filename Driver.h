//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_DRIVER_H
#define C_COMPILER_DRIVER_H

#include "Context.h"
#include "List.h"

typedef struct {
    int argc;
    const char* const* argv;
    Context* context;
} Driver;

typedef struct {
    int error_count;
    int warning_count;
} DiagnosticCount;

/**
 * Creates a new @c Driver .
 * @param argc the same as @c main 's
 * @param argv the same as @c main 's
 * @return a @c Driver handle
 * @retval NULL upon invalid argv (specifically invalid @c SourceText path)
 */
Driver* driver_create(int argc, const char* const* argv);
/**
 * Destroys the @c Driver .
 * @param this handle to the @c Driver
 */
void driver_destroy(Driver* this);
/**
 * compiles the source code
 * @warning ownership of return value transfers to caller.
 * @return TODO: change from List* to other types along with compiler building
 */
List* driver_compile(Driver* this, int source_text_index);

DiagnosticCount driver_get_diagnostic_count(const Driver* this);

void driver_issue_diagnostics(const Driver* this);

#endif //C_COMPILER_DRIVER_H