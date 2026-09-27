//
// Created by tomer on 27-Sep-26.
//

#include "Token.h"

// ---------- token factories ------------- //
static Token token_keyword_create(const TokenKeywordID id) {
    return (Token){.category = TOKEN_CATEGORY_KEYWORD, .as.keyword.id = id};
}
static Token token_identifier_create(const char* const string) {
    int id = list_index_of(globals_identifiers_table, string, list_string_strcmp);
    if (id == -1) {
        id = list_push(globals_identifiers_table, utils_allocate_and_copy_string(string));
    }
    return (Token){.category = TOKEN_CATEGORY_IDENTIFIER, .as.identifier.id = id};
}
static Token token_constant_create(const TokenConstantType type, const int value) {
    return (Token){.category = TOKEN_CATEGORY_CONSTANT, .as.constant = {.type = type, .value = value}};
}
static Token token_string_literal_create(const char* const string) {
    int id = list_index_of(globals_string_literals_table, string, list_string_strcmp);
    if (id == -1) {
        id = list_push(globals_identifiers_table, utils_allocate_and_copy_string(string));
    }
    return (Token){.category = TOKEN_CATEGORY_STRING_LITERAL, .as.string_literal.id = id};
}
static Token token_punctuator_create(const TokenPunctuatorID id) {
    return (Token){.category = TOKEN_CATEGORY_PUNCTUATOR, .as.punctuator.id = id};
}

const Token TOKEN_EMPTY = {.category = TOKEN_CATEGORY_NONE, .as.constant = {.type = TOKEN_CONSTANT_TYPE_INTEGER, .value = 0}};
const Token TOKEN_EOF = {.category = TOKEN_CATEGORY_EOF, .as.constant = {.type = TOKEN_CONSTANT_TYPE_INTEGER, .value = 0}};

bool token_is_EOF(const Token token) {
    return token.category == TOKEN_CATEGORY_EOF;
}