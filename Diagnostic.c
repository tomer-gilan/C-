//
// Created by tomer on 23-Sep-26.
//

#include "Diagnostic.h"

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
    [DIAGNOSTIC_CODE_LEXER_QUOTATION_MARKS_EXPECTED]        = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "unmatched quotation marks"},
    [DIAGNOSTIC_CODE_LEXER_APOSTROPHE_EXPECTED]             = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "unmatched apostrophe"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE]      = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "invalid numerical sequence"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_ESCAPE_SEQUENCE]         = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "invalid escape sequence"},
    [DIAGNOSTIC_CODE_LEXER_NUMERIC_CONSTANT_OUT_OF_BOUNDS]  = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "numerical constant out of bounds"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_PREFIX]        = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "invalid numerical prefix"}
};

static bool diagnostic_is_valid_code(const DiagnosticCode code) {
    return 0 < code && code < DIAGNOSTIC_CODE__COUNT;
}

void diagnostic_issue(const Diagnostic handle) {
    assert(diagnostic_is_valid_code(handle.code) && "Unknown diagnostic code!");

    const FileLineColumn file_line_column = position_get_file_line_column(handle.range.position, handle.context);
    fprintf(stderr,"%s:%d:%d: [%s] %s:",
        file_line_column.file_name, file_line_column.line, file_line_column.column,
        diagnostic_severity_table[diagnostics_table[handle.code].severity],
        diagnostics_table[handle.code].info);
}
void diagnostic_issue_string(const Diagnostic handle, const char* const message) {
    diagnostic_issue(handle);
    fprintf(stderr," %s\n", message);
}
void diagnostic_issue_char(const Diagnostic handle, const char message) {
    const char buff2[2] = {message, '\0'};
    diagnostic_issue_string(handle, buff2);
}
void diagnostic_issue_int(const Diagnostic handle, const int message) {
    char buff[12];
    snprintf(buff, sizeof buff, "%d", message);
    diagnostic_issue_string(handle, buff);
}