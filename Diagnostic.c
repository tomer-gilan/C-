//
// Created by tomer on 23-Sep-26.
//

#include "Diagnostic.h"
#include "Utils.h"

#include <assert.h>
#include <stdio.h>

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

void diagnostic_issue(const Diagnostic report, const Context* const context) {
    assert(diagnostic_is_valid_code(report.code) && "Unknown diagnostic code!");

    //const FileLineColumn file_line_column = file_position_get_file_line_column(report.range.position, context);
    //fprintf(stderr,"%s:%d:%d: [%s] %s:",
    //        file_line_column.file_name, file_line_column.line, file_line_column.column,
    //        diagnostic_severity_table[diagnostics_table[report.code].severity],
    //        diagnostics_table[report.code].info);
}
void diagnostic_issue_string(const Diagnostic report, const Context* const context, const char* const message) {
    diagnostic_issue(report, context);
    fprintf(stderr," %s\n", message);
}
void diagnostic_issue_char(const Diagnostic report, const Context* const context, const char message) {
    const char buff2[2] = {message, '\0'};
    diagnostic_issue_string(report, context, buff2);
}
void diagnostic_issue_int(const Diagnostic report, const Context* const context, const int message) {
    char buff[12];
    snprintf(buff, sizeof buff, "%d", message);
    diagnostic_issue_string(report, context, buff);
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
