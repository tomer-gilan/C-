//
// Created by tomer on 23-Sep-26.
//

#include "Diagnostic.h"

struct Diagnostic{
    DiagnosticSeverity severity;
    char* info;  // the problematic lexeme/token/etc, as a string.
};

static const char* const diagnostic_severity_table[DIAGNOSTIC_SEVERITY__COUNT] = {
    [DIAGNOSTIC_SEVERITY_NOTE]      = "NOTE",
    [DIAGNOSTIC_SEVERITY_WARNING]   = "WARNING",
    [DIAGNOSTIC_SEVERITY_ERROR]     = "ERROR",
    [DIAGNOSTIC_SEVERITY_FATAL]     = "FATAL"
};
static const Diagnostic diagnostics_table[DIAGNOSTIC_CODE__COUNT] = {
    [DIAGNOSTIC_CODE_DIAGNOSTIC_UNKNOWN_DIAGNOSTIC_CODE]        = {.severity = DIAGNOSTIC_SEVERITY_FATAL, .info = "Unknown diagnostic code!"},
    [DIAGNOSTIC_CODE_LEXER_UNKNOWN_CHARACTER]                   = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: Unknown character"},
    [DIAGNOSTIC_CODE_LEXER_UNMATCHED_QUOTATION_MARKS]           = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: Unmatched quotation marks"},
    [DIAGNOSTIC_CODE_LEXER_UNMATCHED_APOSTROPHE]                = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: Unmatched apostrophe"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE]          = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: Invalid numerical sequence"},
    [DIAGNOSTIC_CODE_LEXER_QUOTATION_MARKS_EXPECTED]            = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: quotation marks expected"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_ESCAPE_CHARACTER]            = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: the following character cannot be escaped"},
    [DIAGNOSTIC_CODE_LEXER_MISSING_DELIMITER]                   = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: missing the following delimiter"},
    [DIAGNOSTIC_CODE_INDEX_OUT_OF_BOUNDS]                       = {.severity = DIAGNOSTIC_SEVERITY_FATAL, .info = "the following index is out of bounds"},
    [DIAGNOSTIC_CODE_LEXER_STRUCTURAL_CHARACTER_ENCOUNTERED]    = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: encountered a structural character (MSB=1) in source code"},
    [DIAGNOSTIC_CODE_LEXER_NUMERIC_SEQUENCE_OUT_OF_BOUNDS]      = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: the following numeric sequence is out of bounds"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SUFFIX]            = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: the following numerical suffix is invalid"},
    [DIAGNOSTIC_CODE_UNINITIALIZED_TABLE]                       = {.severity = DIAGNOSTIC_SEVERITY_FATAL, .info = "LEXER: the following table has not been initialized"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_PUNCTUATOR_SEQUENCE]         = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: invalid punctuators sequence"},
    [DIAGNOSTIC_CODE_LEXER_UNEXPECTED_EOS]                      = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: unexpected EOS"},
    [DIAGNOSTIC_CODE_LEXER_INVALID_CHARACTER_LITERAL_SEQUENCE]  = {.severity = DIAGNOSTIC_SEVERITY_ERROR, .info = "LEXER: invalid character literal sequence"},
    [DIAGNOSTIC_CODE_INVALID_INTERNAL_STATE]                    = {.severity = DIAGNOSTIC_SEVERITY_FATAL, .info = "invalid compiler internal state"},
};

static bool diagnostic_is_valid_code(const DiagnosticCode code) {
    return 0 < code && code < DIAGNOSTIC_CODE__COUNT;
}

void diagnostic_issue(const DiagnosticCode code, const Position position) {
    if (!diagnostic_is_valid_code(code)) {
        diagnostic_issue(DIAGNOSTIC_CODE_DIAGNOSTIC_UNKNOWN_DIAGNOSTIC_CODE, (Position){.line = 0, .column = 0}); // A bit funny, but could theoretically happen...
        return;
    }
    fprintf(stderr,"[%s]:%d:%d\t%s.\n",
        diagnostic_severity_table[diagnostics_table[code].severity],
        position.line,
        position.column,
        diagnostics_table[code].info);
}
void diagnostic_issue_string(const DiagnosticCode code, const Position position, const char* const additional_info) {
    diagnostic_issue(code, position);
    fprintf(stderr,"Additional information: %s.\n", additional_info);
}
void diagnostic_issue_char(const DiagnosticCode code, const Position position, const char additional_info) {
    const char buff2[2] = {additional_info, '\0'};
    diagnostic_issue_string(code, position, buff2);
}
void diagnostic_issue_int(const DiagnosticCode code, const Position position, const int additional_info) {
    char buff[12];
    snprintf(buff, sizeof buff, "%d", additional_info);
    diagnostic_issue_string(code, position, buff);
}