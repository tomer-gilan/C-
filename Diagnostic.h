//
// Created by tomer on 23-Sep-26.
//

#ifndef C_COMPILER_ERROR_H
#define C_COMPILER_ERROR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "Range.h"
#include "Context.h"

typedef enum {
    DIAGNOSTIC_SEVERITY_NOTE,
    DIAGNOSTIC_SEVERITY_WARNING,
    DIAGNOSTIC_SEVERITY_ERROR,
    DIAGNOSTIC_SEVERITY_FATAL,
    DIAGNOSTIC_SEVERITY__COUNT
} DiagnosticSeverity;
typedef enum {
    DIAGNOSTIC_CODE_LEXER_UNKNOWN_CHARACTER,
    DIAGNOSTIC_CODE_LEXER_QUOTATION_MARKS_EXPECTED,
    DIAGNOSTIC_CODE_LEXER_APOSTROPHE_EXPECTED,
    DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE,
    DIAGNOSTIC_CODE_LEXER_INVALID_ESCAPE_SEQUENCE,
    DIAGNOSTIC_CODE_LEXER_NUMERIC_CONSTANT_OUT_OF_BOUNDS,
    DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_PREFIX,
    // sentinel:
    DIAGNOSTIC_CODE__COUNT
} DiagnosticCode;

typedef struct DiagnosticDescriptor DiagnosticDescriptor;
typedef struct {
    DiagnosticCode code;
    Range range;            ///< the place the @c error / @c warning took place.
    Context* context;
} Diagnostic;

void diagnostic_issue(Diagnostic handle);
void diagnostic_issue_string(Diagnostic handle, const char* message);
void diagnostic_issue_char(Diagnostic handle, char message);
void diagnostic_issue_int(Diagnostic handle, int message);

#endif //C_COMPILER_ERROR_H