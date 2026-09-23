//
// Created by tomer on 22-Sep-26.
//

#ifndef GIT_CODE_LEXER_H
#define GIT_CODE_LEXER_H

#include <stdio.h>

//     ----- General Definitions -----     //
typedef struct {
    char** entries;
    int size;
} string_array;
//     ----- END: General Definitions -----     //


//     ----- Token Categories -----     //
typedef enum {
    KEYWORD = 0,
    IDENTIFIER,
    CONSTANT,
    STRING_LITERAL,
    PUNCTUATORS
} token_category;
//     ----- END: Token Categories -----     //


//     ----- Keywords -----     //
const char* const keywords_table[] = {
    "char",
    "else",
    "if",
    "int",
    "return",
    "void",
    "while"
};
typedef struct {
    int id;
} keyword;
//     ----- END: Keywords -----     //


//     ----- Identifiers -----     //
string_array identifiers_table = {.entries = NULL, .size = 0};
typedef struct {
    int id; // the index in the identifiers_table
} identifier;
//     ----- END: Identifiers -----     //


//     ----- Constants -----     //
typedef enum {
    INTEGER = 0,
    CHARACTER
} token_constant_type;
typedef struct {
    token_constant_type type;
    int raw_value; // inferred based on type.
} constant;
//     ----- END: Constants -----     //


//     ----- String Literals -----     //
string_array string_literals_table = {.entries = NULL, .size = 0};
typedef struct {
    int id; // the index in the string_literals_table
} string_literal;
//     ----- END: String Literals -----     //


//     ----- Punctuators -----     //
const char* punctuators[] = {
    "[", "]", "(", ")",
    "{", "}", "&", "*",
    "+", "-", "~", "!",
    "/", "%", "^", "=",
    "<<", ">>", ">", "<",
    "<=", ">=", "==", "!=",
    "|", "&&", "||", ";"
};
//     ----- END: Punctuators -----     //

typedef struct {
    token_category category;
    void* data;
} token;

#endif //GIT_CODE_LEXER_H