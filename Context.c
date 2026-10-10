//
// Created by tomer on 27-Sep-26.
//

#include <assert.h>
#include <stdlib.h>

#include "Context.h"
#include "Utils.h"

Context* context_create(SourceText* const source) {
    assert(source != NULL && "Invalid source!");

    Context* context = utils_xmalloc(sizeof (Context));
    *context = (Context){
        .identifiers_table      = list_create(free),
        .string_literals_table  = list_create(text_list_destroy),
        .source_text_table = list_create(source_text_list_destroy),
        .diagnostic_report_table = list_create(free)
    };
    list_push(context->source_text_table, source);
    return context;
}
void context_destroy(Context* const this) {
    assert(this->identifiers_table != NULL && "table is NULL");
    list_destroy(this->identifiers_table);
    this->identifiers_table = NULL;

    assert(this->string_literals_table != NULL && "table is NULL");
    list_destroy(this->string_literals_table);
    this->string_literals_table = NULL;

    assert(this->source_text_table != NULL && "table is NULL");
    list_destroy(this->source_text_table);
    this->source_text_table = NULL;

    assert(this->diagnostic_report_table != NULL && "table is NULL");
    list_destroy(this->diagnostic_report_table);
    this->diagnostic_report_table = NULL;

    free(this);
}
SourceText* context_source_text_at(Context* const this, const int index) {
    return list_at(this->source_text_table, index);
}
const SourceText* context_source_text_at_const(const Context* const this, const int index) {
    return list_at(this->source_text_table, index);
}

void context_report_diagnostic(Context * const this, const Diagnostic report) {
    list_push(this->diagnostic_report_table, diagnostic_list_duplicate(&report));
}
