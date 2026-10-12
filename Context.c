//
// Created by tomer on 27-Sep-26.
//

#include "Context.h"
#include "Utils.h"
#include "Diagnostic.h"

#include <assert.h>
#include <stdlib.h>

CompilationFlags compilation_flags_from_arguments(const int argc, const char* const* const argv) {
    // TODO: implement argv parsing
    return (CompilationFlags)
    {
        .pedantic = false
    };
}

CompilationFlags compilation_flags_basic() {
    // TODO: reuse 'compilation_flags_from_arguments'
    return (CompilationFlags)
    {
        .pedantic = false
    };
}

Context* context_create(SourceText * const source, const CompilationFlags flags) {
    assert(source != NULL && "Invalid source!");

    Context* context = utils_xmalloc(sizeof (Context));
    *context = (Context){
        .identifiers_table      = list_create(free),
        .string_literals_table  = list_create(text_list_destroy),
        .source_text_table = list_create(source_text_list_destroy),
        .diagnostic_report_table = list_create(free),
        .flags = flags
    };
    list_push(context->source_text_table, source);
    return context;
}
void context_destroy(Context* const this) {
    assert(this != NULL);

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
    assert(this != NULL);
    return list_at(this->source_text_table, index);
}
TextView context_range_to_text_view(const Context* const this, const Range range) {
    assert(this != NULL);
    assert(range.length >= 0 && "invalid range length (negative)");
    const SourceText* const source_text = context_source_text_at_const(this, range.position.source_text_index);
    assert(source_text != NULL);
    assert(source_text_is_index_in_bounds(source_text, range.position.offset + range.length - 1)
        && "invalid range (outside of SourceText bounds)");
    return (TextView)
    {
        .data = source_text->data->data + range.position.offset,
        .length = range.length
    };
}

int context_intern_identifier(Context* const this, const TextView spelling) {
    assert(this != NULL);
    return list_find_or_push(
        this->identifiers_table,
        &spelling,
        text_view_list_compare,
        text_view_list_duplicate);
}

int context_intern_string_literal(Context* const this, const Text* const text) {
    return list_find_or_push(
        this->string_literals_table,
        text,
        text_list_compare,
        text_list_duplicate);
}

void context_report_diagnostic(Context * const this, const Diagnostic report) {
    list_push(this->diagnostic_report_table, diagnostic_list_duplicate(&report));
}
