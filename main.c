
#include <stdio.h>
#include <stdlib.h>

#include "Diagnostic.h"

int main() {
    Error* err = error_create(ERROR_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE, "01234");
    error_print(err);
    error_destroy(err);

    return 0;
}