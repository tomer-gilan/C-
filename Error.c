//
// Created by tomer on 23-Sep-26.
//

#include "Error.h"

struct Error {
    ErrorCode code;
    char* info;  // the problematic lexeme/token/etc, as a string.
};

char* error_messages[] = {
    "LEXER: Unknown character",
    "LEXER: Unmatched quotation marks",
    "LEXER: Unmatched apostrophe",
    "LEXER: Invalid numerical sequence",
};

Error* error_create(const ErrorCode code, const char* const info) {
    Error* error = malloc(sizeof (Error));
    error->code = code;
    error->info = utils_allocate_and_copy(info);
    return error;
}
void error_destroy(Error* const this) {
    free(this->info);
    free(this);
}
void error_print(const Error* const this) {
    printf("Error: %s.\nAdditional Info: %s.", error_messages[this->code], this->info);
}