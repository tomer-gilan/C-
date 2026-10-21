//
// Created by tomer on 27-Sep-26.
//

#include "Context.h"
#include "Utils.h"
#include "Diagnostic.h"
#include "Arena.h"

#include <assert.h>
#include <stdlib.h>

/**
 * @param to_destroy type: void*, meaning: the element to be destroyed.
 * @param destroyer type: void (*)(void*), meaning: a function that takes a pointer and destroys it.
 */
#define assert_destroy_and_nullify(to_destroy, destroyer)   \
    do                                                      \
    {                                                       \
        assert(to_destroy != NULL);                         \
        destroyer(to_destroy);                              \
        to_destroy = NULL;                                  \
    }                                                       \
    while (false)                                           \

CompilationArguments compilation_flags_from_arguments(const int argc, const char* const* const argv) {
    // TODO: implement argv parsing
    assert(argc == 3);
    return (CompilationArguments){
        .pedantic = false,
        .destination_file = argv[2]
    };
}
//CompilationArguments compilation_flags_basic(void) {
//    // TODO: reuse 'compilation_flags_from_arguments'
//    return compilation_flags_from_arguments(0, NULL);
//}
Context* context_create(SourceText * const source, const CompilationArguments flags) {
    assert(source != NULL && "Invalid source!");

    Context* context = utils_xmalloc(sizeof (Context));
    *context = (Context){
        .identifiers_table          = list_create(free),
        .string_literals_table      = list_create(text_list_destroy),
        .source_text_table          = list_create(source_text_list_destroy),
        .diagnostic_report_table    = list_create(free),
        .arena                      = arena_create(),
        .compilation_arguments                      = flags
    };
    list_push_last(context->source_text_table, source);
    return context;
}
void context_destroy(Context* const this) {
    assert(this != NULL);

    assert_destroy_and_nullify(this->identifiers_table, list_destroy);
    assert_destroy_and_nullify(this->string_literals_table, list_destroy);
    assert_destroy_and_nullify(this->source_text_table, list_destroy);
    assert_destroy_and_nullify(this->diagnostic_report_table, list_destroy);

    assert_destroy_and_nullify(this->arena, arena_destroy);

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
        text_view_list_equals,
        text_view_list_duplicate);
}
int context_intern_string_literal(Context* const this, const Text* const text) {
    return list_find_or_push(
        this->string_literals_table,
        text,
        text_list_equals,
        text_list_duplicate);
}
void context_report_diagnostic(Context* const this, const DiagnosticCode code, const Range range) {
    const Diagnostic report = diagnostic_make(this, code, range);
    list_push_last(this->diagnostic_report_table, diagnostic_list_duplicate(&report));
}
void context_print_range(const Context* const this, const Range to_print, FILE* const output) {
    text_view_print(context_range_to_text_view(this, to_print), output);
}
