//
// Created by tomer on 23-Sep-26.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

Error* error_create(const ErrorCode code, const char* info) {
    char* info_dynamic_copy = malloc(sizeof(char) * (strlen(info) + 1));
    strcpy(info_dynamic_copy, info);

    Error* error = malloc(sizeof (Error));
    error->code = code;
    error->info = info_dynamic_copy;
    return error;
}
void error_destroy(Error* error) {
    free(error->info);
    free(error);
}
void error_print(const Error* error) {
    printf("Error: %s.\nAdditional Info: %s.", error_messages[error->code], error->info);
}