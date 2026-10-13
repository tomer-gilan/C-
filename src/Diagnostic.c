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

static LineColumn diagnostic_get_line_column(const Context* const context, const FilePosition file_position) {
    LineColumn to_return = {.line = 0, .column = 0};
    const SourceText* const source_text = context_source_text_at_const(context, file_position.source_text_index);
    for (int i = 0; i < file_position.offset; ++i)
        if (source_text->data->data[i] == '\n') {
            to_return.line++;
            to_return.column = 0;
        }
        else
            to_return.column++;
    return to_return;
}
static bool diagnostic_is_valid_code(const DiagnosticCode code) {
    return code < DIAGNOSTIC_CODE__COUNT;
}
Diagnostic diagnostic_make(const Context* const context, const DiagnosticCode code, const Range range) {
    return (Diagnostic){
        .code = code,
        .range = range,
        .line_column = diagnostic_get_line_column(context, range.position)
    };
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
void static diagnostic_print_marker(const Diagnostic report) {
    for (int i = 0; i < report.line_column.column; ++i)
        printf(" ");
    bool first = true;
    for (int i = 0; i < report.range.length; ++i)
        if (first) {
            first = false;
            printf("^");
        }
        else
            printf("~");
}
void static diagnostic_print_source_text_row(const Diagnostic report, const Context* const context) {
    // get beg of line pos
    FilePosition beginning_of_line_position = report.range.position;
    beginning_of_line_position.offset -= report.line_column.column;

    // set range starting from beg of line
    Range range_to_print = {
        .position = beginning_of_line_position,
        .length = report.line_column.column
    };

    // extend until EOF or end-of-line.
    const SourceText* source_text = context_source_text_at_const(context, report.range.position.source_text_index);
    for (int i = report.range.position.offset; i < source_text->data->length && source_text->data->data[i] != '\n'; ++i)
        range_extend(&range_to_print, 1);

    // print range
    context_print_range(context, range_to_print);
}
void diagnostic_print(const Diagnostic report, const Context* const context) {
    assert(diagnostic_is_valid_code(report.code) && "Unknown diagnostic code!");

    const char* const file_name = context_source_text_at_const(context, report.range.position.source_text_index)->name;
    printf("%s:%d:%d: [%s] %s.\n",
            file_name,
            report.line_column.line,
            report.line_column.column,
            diagnostic_get_severity_string(report),
            diagnostic_get_info(report));
    printf("|");
    diagnostic_print_source_text_row(report, context);
    printf("|");
    diagnostic_print_marker(report);
    printf("\n");
}

/*
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
*/
bool diagnostic_equals(const Diagnostic first, const Diagnostic second) {
    return first.code == second.code && range_equals(first.range, second.range);
}
int diagnostic_compare(const Diagnostic first, const Diagnostic second) {
    if (diagnostic_equals(first, second))
        return 0;
    {
        const int first_severity = diagnostic_get_severity(first);
        const int second_severity = diagnostic_get_severity(second);
        if (first_severity != second_severity)
            return first_severity - second_severity;
    }
    // same severity, so going by range
    return range_compare(first.range, second.range);
}
bool diagnostic_list_equals(const void* const first, const void* const second) {
    return diagnostic_equals(*(Diagnostic *) first, *(Diagnostic *) second);
}
void *diagnostic_list_duplicate(const void* const element) {
    Diagnostic* const to_return = utils_xmalloc(sizeof(Diagnostic));
    *to_return = *(Diagnostic *) element;
    return to_return;
}
int diagnostic_list_compare(const void* const first, const void* const second) {
    return diagnostic_compare(*(Diagnostic*)first, *(Diagnostic*)second);
}
