//
// Created by tomer on 27-Sep-26.
//

#include <assert.h>
#include <stdlib.h>

#include "Context.h"

Context* context_create(const char* source_file_path) {
    SourceFile* const source = source_file_create(source_file_path);
    if (source == NULL)
        return NULL;
    Context* context = malloc(sizeof (Context));
    *context = (Context){
        .identifiers_table      = list_create(free),
        .string_literals_table  = list_create(free),
        .source_file_table      = list_create(source_file_list_destroy)
    };
    list_push(context->source_file_table, source);
    return context;
}
void context_destroy(Context* const this) {
    assert(this->identifiers_table != NULL && "table is NULL");
    list_destroy(this->identifiers_table);
    this->identifiers_table = NULL;

    assert(this->string_literals_table != NULL && "table is NULL");
    list_destroy(this->string_literals_table);
    this->string_literals_table = NULL;

    assert(this->source_file_table != NULL && "table is NULL");
    list_destroy(this->source_file_table);
    this->source_file_table = NULL;

    free(this);
}
SourceFile* context_source_file_at(Context* const this, const int index) {
    return list_at(this->source_file_table, index);
}
const SourceFile* context_source_file_at_const(const Context* const this, const int index) {
    return list_at(this->source_file_table, index);
}