//
// Created by tomer on 27-Sep-26.
//

#include "Token.h"

const Token TOKEN_EOF = {.kind = TOKEN_KIND_EOF};

bool token_is_EOF(const Token token) {
    return token.kind == TOKEN_KIND_EOF;
}