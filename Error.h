//
// Created by tomer on 23-Sep-26.
//

#ifndef GIT_CODE_ERROR_H
#define GIT_CODE_ERROR_H

typedef enum {
    ERROR_CODE_LEXER_UNKNOWN_CHARACTER,
    ERROR_CODE_LEXER_UNMATCHED_QUOTATION_MARKS,
    ERROR_CODE_LEXER_UNMATCHED_APOSTROPHE,
    ERROR_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE
} ErrorCode;
typedef struct Error Error; // user abstraction

/**
 * Creates a new error, and returns the handle.
 * @param code the error code
 * @param info additional, DYNAMICALLY ALLOCATED info (lexeme, token, etc.).
 * Ownership transfers to error, to be handled via error_destroy later
 * @return A handle to the error
 */
Error* error_create(ErrorCode code, const char* info);

/**
 * Frees memory, and handles all destroying related operations. Must be called for every Error* created via error_create.
 * @param error a handle to the error
 */
void error_destroy(Error* error);

/**
 * prints the error message
 * @param error error handle
 */
void error_print(const Error* error);

#endif //GIT_CODE_ERROR_H