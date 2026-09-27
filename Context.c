//
// Created by tomer on 27-Sep-26.
//

#include "Context.h"

Context* context_create(const char* source_file_path) {
    Context* context = malloc(sizeof (Context));
    *context = (Context){
        .identifiers_table      = list_create(NULL),
        .string_literals_table  = list_create(NULL),
        .source_file_table      = list_create(NULL)
    };
    return context;
}
void context_destroy(Context* const this) {
    list_destroy(this->identifiers_table);
    this->identifiers_table = NULL;

    list_destroy(this->string_literals_table);
    this->string_literals_table = NULL;

    list_destroy(this->source_file_table);
    this->source_file_table = NULL;

    free(this);
}