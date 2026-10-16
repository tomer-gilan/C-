//
// Created by tomer on 23-Sep-26.
//

#ifndef C_COMPILER_DIAGNOSTIC_H
#define C_COMPILER_DIAGNOSTIC_H

typedef struct Context Context; // forward declaration
#include "data_structures/Range.h"

typedef struct {
    int line;
    int column;
} LineColumn;

typedef enum {
    DIAGNOSTIC_SEVERITY_FATAL,
    DIAGNOSTIC_SEVERITY_ERROR,
    DIAGNOSTIC_SEVERITY_WARNING,
    DIAGNOSTIC_SEVERITY_NOTE,
    DIAGNOSTIC_SEVERITY__COUNT
} DiagnosticSeverity;
typedef enum {
    DIAGNOSTIC_CODE_LEXER_UNKNOWN_CHARACTER,
    DIAGNOSTIC_CODE_LEXER_EXPECTED_QUOTATION_MARKS,
    DIAGNOSTIC_CODE_LEXER_EXPECTED_APOSTROPHE,
    DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE,
    DIAGNOSTIC_CODE_LEXER_INVALID_ESCAPE_SEQUENCE,
    DIAGNOSTIC_CODE_LEXER_NUMERIC_CONSTANT_OUT_OF_BOUNDS,
    DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_PREFIX,
    DIAGNOSTIC_CODE_LEXER_UNEXPECTED_EOF,
    DIAGNOSTIC_CODE_LEXER_EXPECTED_EXPRESSION,
    DIAGNOSTIC_CODE_LEXER_MULTI_CHARACTER_LITERAL,
    DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN,
    DIAGNOSTIC_CODE_PARSER_EXPECTED_COMPOUND_STATEMENT,
    DIAGNOSTIC_CODE_PARSER_EXPECTED_EXTERNAL_FUNCTION_DEFINITION,
    DIAGNOSTIC_CODE_PARSER_INVALID_EXPRESSION,
    DIAGNOSTIC_CODE_PARSER_INVALID_STATEMENT,
    DIAGNOSTIC_CODE_PARSER_MISSING_EXTERNAL_DEFINITION,
    // sentinel:
    DIAGNOSTIC_CODE__COUNT
} DiagnosticCode;

typedef struct DiagnosticDescriptor DiagnosticDescriptor;
typedef struct {
    DiagnosticCode code;
    Range range; ///< the place the @c error / @c warning took place.
    LineColumn line_column;
} Diagnostic;

Diagnostic diagnostic_make(const Context* context, DiagnosticCode code, Range range);
DiagnosticSeverity diagnostic_get_severity(Diagnostic report);
const char *diagnostic_get_severity_string(Diagnostic report);
const char *diagnostic_get_info(Diagnostic report);
void diagnostic_print(Diagnostic report, const Context* context);
/*
void diagnostic_print_string(Diagnostic report, const Context* context, const char* message);
void diagnostic_print_char(Diagnostic report, const Context* context, char message);
void diagnostic_print_int(Diagnostic report, const Context* context, int message);
*/
bool diagnostic_equals(Diagnostic first, Diagnostic second);
int diagnostic_compare(Diagnostic first, Diagnostic second);
// list compliance:
bool diagnostic_list_equals(const void* first, const void* second);
void *diagnostic_list_duplicate(const void* element);
int diagnostic_list_compare(const void* first, const void* second);

#endif //C_COMPILER_DIAGNOSTIC_H