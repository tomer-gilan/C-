//
// Created by tomer on 27-Sep-26.
//

#ifndef GIT_CODE_GLOBALS_H
#define GIT_CODE_GLOBALS_H

#include "List.h"
#include "Location.h"

typedef struct {
    List* identifiers_table;
    List* string_literals_table;
    SourceFile source;
} Context;

Context* context_create();
void context_destroy(Context* this);

#endif //GIT_CODE_GLOBALS_H