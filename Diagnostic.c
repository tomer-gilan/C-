//
// Created by tomer on 23-Sep-26.
//

#include "Diagnostic.h"
#include "Utils.h"

#include <assert.h>
#include <stdio.h>

#include "Context.h"

struct DiagnosticDescriptor {
    DiagnosticSeverity severity;
    const char* info;  // the problematic lexeme/token/etc, as a string.
};

static const char* const diagnostic_severity_table[DIAGNOSTIC_SEVERITY__COUNT] = {
    [DIAGNOSTIC_SEVERITY_NOTE]      = "NOTE",
    [DIAGNOSTIC_SEVERITY_WARNING]   = "WARNING",
    [DIAGNOSTIC_SEVERITY_ERROR]     = "ERROR",
    [DIAGNOSTIC_SEVERITY_FATAL]     = "FATAL"
};
static const DiagnosticDescriptor diagnostics_table[DIAGNOSTIC_CODE__COUNT] = {
    [DIAGNOSTIC_CODE_LEXER_UNKNOWN_CHARACTER]               = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "unknown character"},
    [DIAGNOSTIC_CODE_LEXER_EXPECTED_QUOTATION_MARKS] = {
        .severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "unmatched quotation marks"
    },
    [DIAGNOSTIC_CODE_LEXER_EXPECTED_APOSTROPHE] = {
        .severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "unmatched apostrophe"
    },
    [DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE]      = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "invalid numerical sequence"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_ESCAPE_SEQUENCE]         = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "invalid escape sequence"},
    [DIAGNOSTIC_CODE_LEXER_NUMERIC_CONSTANT_OUT_OF_BOUNDS]  = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "numerical constant out of bounds"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_PREFIX] = {
        .severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "invalid numerical prefix"
    },
    [DIAGNOSTIC_CODE_LEXER_UNEXPECTED_EOF] = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "unexpected end-of-file"},
    [DIAGNOSTIC_CODE_LEXER_EXPECTED_EXPRESSION] = {
        .severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "expected expression"
    },
    [DIAGNOSTIC_CODE_LEXER_MULTI_CHARACTER_LITERAL] = {
        .severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "multi character literal"
    },
};

static bool diagnostic_is_valid_code(const DiagnosticCode code) {
    return code < DIAGNOSTIC_CODE__COUNT;
}
DiagnosticSeverity diagnostic_get_severity(const Diagnostic report) {
    assert(diagnostic_is_valid_code(report.code));
    return diagnostics_table[report.code].severity;
}

const char *diagnostic_get_severity_string(Diagnostic report) {
    return diagnostic_severity_table[diagnostic_get_severity(report)];
}

const char *diagnostic_get_info(const Diagnostic report) {
    assert(diagnostic_is_valid_code(report.code));
    return diagnostics_table[report.code].info;
}

LineColumn diagnostic_get_line_column(const Diagnostic report, const Context* const context) {
    int line_counter = 0, column_counter = 0;
    const SourceText* const source_text =
            context_source_text_at_const(context, report.range.position.source_text_index);
    for (int i = 0; i < report.range.length; ++i)
        if (source_text->data->data[report.range.position.offset + i] == '\n')
            line_counter++, column_counter = 0;
        else
            column_counter++;
    return (LineColumn)
    {
        .line = line_counter, .column = column_counter
    };
}

void diagnostic_print(const Diagnostic report, const Context* const context) {
    assert(diagnostic_is_valid_code(report.code) && "Unknown diagnostic code!");

    const char* const file_name = context_source_text_at_const(context, report.range.position.source_text_index)->name;
    const LineColumn line_column = diagnostic_get_line_column(report, context);

    fprintf(stderr, "%s:%d:%d: [%s] %s:",
            file_name,
            line_column.line,
            line_column.column,
            diagnostic_get_severity_string(report),
            diagnostic_get_info(report));
}
void diagnostic_print_string(const Diagnostic report, const Context* const context, const char* const message) {
    diagnostic_print(report, context);
    fprintf(stderr," %s\n", message);
}
void diagnostic_print_char(const Diagnostic report, const Context* const context, const char message) {
    const char buff[2] = {message, '\0'};
    diagnostic_print_string(report, context, buff);
}
void diagnostic_print_int(const Diagnostic report, const Context* const context, const int message) {
    char buff[12];
    snprintf(buff, sizeof buff, "%d", message);
    diagnostic_print_string(report, context, buff);
}
int diagnostic_compare(const Diagnostic first, const Diagnostic second) {
    return first.code == second.code && range_equals(first.range, second.range) ? 0 : 1;
}
int diagnostic_list_compare(const void* const first, const void* const second) {
    return diagnostic_compare(*(Diagnostic *) first, *(Diagnostic *) second);
}
void *diagnostic_list_duplicate(const void* const element) {
    Diagnostic* const to_return = utils_xmalloc(sizeof(Diagnostic));
    *to_return = *(Diagnostic *) element;
    return to_return;
}
