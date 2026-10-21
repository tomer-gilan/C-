//
// Created by tomer on 27-Sep-26.
//

#include "Driver.h"
#include "Context.h"
#include "Lexer.h"
#include "Utils.h"
#include "Codegen.h"
#include "Parser.h"
#include "Sema.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


static void driver_preprocess_source_text(Driver* const this, const int source_text_index) {
    SourceText* const source_text = context_source_text_at(this->context, source_text_index);
    Text* const processed = text_create_empty();

    for (int i = 0; i < source_text->data->length; ++i) {
        const char c = source_text->data->data[i];
        if (c == '\r') {
            if (i + 1 == source_text->data->length) {
                // TODO: issue a diagnostic and return
            }
        }
        else
            text_append_char(processed, c);
    }
    text_destroy(source_text->data);
    source_text->data = processed;
}
static DiagnosticCount driver_get_diagnostic_count(const Driver* this) {
    DiagnosticCount report_count = {.error_count = 0, .warning_count = 0};
    const int N = list_get_size(this->context->diagnostic_report_table);
    for (int i = 0; i < N; ++i) {
        const DiagnosticSeverity severity = diagnostic_get_severity(
            *(const Diagnostic*)list_at_const(this->context->diagnostic_report_table, i));
        switch (severity) {
            case DIAGNOSTIC_SEVERITY_ERROR:
                report_count.error_count++;
                break;
            case DIAGNOSTIC_SEVERITY_WARNING:
                if (this->context->compilation_arguments.pedantic)
                    report_count.error_count++;
                else
                    report_count.warning_count++;
                break;
            case DIAGNOSTIC_SEVERITY_FATAL: // currently unused
            case DIAGNOSTIC_SEVERITY_NOTE:  // currently unused
                break;
            case DIAGNOSTIC_SEVERITY__COUNT:
            default:
                assert(false && "invalid input");
                break;
        }
    }
    return report_count;
}
static void driver_print_diagnostic_count(const Driver* const this) {
    const DiagnosticCount count = driver_get_diagnostic_count(this);
    fprintf(stderr,"Encountered %d errors, %d warnings.\n", count.error_count, count.warning_count);
}
/**
 * @attention clears @c Context 's diagnostic table
 */
static void driver_issue_diagnostics(Driver* const this) {
    driver_print_diagnostic_count(this);

    List* const diagnostic_report_table = this->context->diagnostic_report_table;
    list_sort(diagnostic_report_table, diagnostic_list_compare, true);

    const int N = list_get_size(diagnostic_report_table);
    for (int i = 0; i < N; ++i)
        diagnostic_print(*(const Diagnostic*)list_at_const(diagnostic_report_table, i), this->context);

    list_clear(diagnostic_report_table);
}

Driver* driver_create(const int argc, const char* const* argv) {
    if (argc <= 1) {
        // TODO: issue a diagnostic "missing source file". note that currently there's no way to do it without a context.
        exit(1); // fatal diagnostic, so exit(1).
    }

    SourceText* const source_text = source_text_create_from_path(argv[1]); // TODO: change hard-wiring to command line argument parsing.
    if (source_text == NULL)
        return NULL; // as the docs specify.
    Context* const context = context_create(source_text, compilation_flags_from_arguments(argc, argv));

    Driver* driver = utils_xmalloc(sizeof (Driver));
    *driver = (Driver){
        .argc = argc,
        .argv = argv,
        .context = context
    };
    return driver;
}
void driver_destroy(Driver* this) {
    context_destroy(this->context);
    free(this);
}
static bool driver_encountered_errors(const Driver* const this) {
    const DiagnosticCount count = driver_get_diagnostic_count(this);
    return count.error_count > 0;
}
bool driver_compile(Driver* const this, const int source_text_index) {
    bool encountered_errors = false;
    driver_preprocess_source_text(this, source_text_index);
    // Lexing:
    List* tokens = NULL;
    if (!encountered_errors) {
        Lexer* lexer = lexer_create(this->context, source_text_index);
        tokens = lexer_lex_source_text(lexer);
        lexer_destroy(lexer);
        if (driver_encountered_errors(this))
            encountered_errors = true;
    }
    // Parsing:
    const TranslationUnitNode* ast = NULL;
    int expression_count = 0;
    if (!encountered_errors) {
        Parser* const parser = parser_create(tokens, this->context);
        ast = parser_parse(parser);
        expression_count = parser_get_expression_id_count(parser);
        parser_destroy(parser);
        if (driver_encountered_errors(this))
            encountered_errors = true;
    }
    // Sema:
    SemaResult sema_result;
    if (!encountered_errors) {
        Sema* const sema = sema_create(this->context, ast, expression_count);
        sema_result = sema_analyze(sema);
        sema_destroy(sema);
        if (driver_encountered_errors(this))
            encountered_errors = true;
    }
    // Code gen:
    if (!encountered_errors) {
        Codegen* codegen = codegen_create(this->context, ast, sema_result);
        Text* assembly = codegen_generate(codegen);
        text_write_to_path(assembly, this->context->compilation_arguments.destination_file);
        text_destroy(assembly);
        codegen_destroy(codegen);
    }

    driver_issue_diagnostics(this);
    list_destroy(tokens);

    return !encountered_errors;
}