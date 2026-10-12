//
// Created by tomer on 27-Sep-26.
//

#include "Driver.h"
#include "Context.h"
#include "Lexer.h"
#include "Utils.h"

#include <stdlib.h>

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
List* driver_compile(Driver* const this, const int source_text_index) {
    Lexer* lexer = lexer_create(this->context, source_text_index);
    List* const tokens = lexer_lex_source_text(lexer);
    lexer_destroy(lexer);


    return tokens;
}

DiagnosticCount driver_get_diagnostic_count(const Driver* this) {
    DiagnosticCount report_count = {.error_count = 0, .warning_count = 0};
    const int N = list_get_size(this->context->diagnostic_report_table);
    for (int i = 0; i < N; ++i) {
        const DiagnosticSeverity severity = diagnostic_get_severity(
            *(Diagnostic *) list_at_const(this->context->diagnostic_report_table, i));
        switch (severity) {
            case DIAGNOSTIC_SEVERITY_ERROR:
                report_count.error_count++;
                break;
            case DIAGNOSTIC_SEVERITY_WARNING:
                if (this->context->flags.pedantic)
                    report_count.error_count++;
                else
                    report_count.warning_count++;
                break;
            default:
                break;
        }
    }
    return report_count;
}

void driver_issue_diagnostics(const Driver* this) {
    // TODO: list_clear at the end, and list_sort at the beginning.
    const int N = list_get_size(this->context->diagnostic_report_table);
    for (int i = 0; i < N; ++i) {
        const Diagnostic* diagnostic = list_at_const(this->context->diagnostic_report_table, i);
        diagnostic_print(*diagnostic, this->context);
    }
}
