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
} StringArray;
//     ----- END: General Definitions -----     //


//     ----- Token Categories -----     //
typedef enum {
    KEYWORD = 0,
    IDENTIFIER,
    CONSTANT,
    STRING_LITERAL,
    PUNCTUATORS
} TokenCategory;
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
} Keyword;
//     ----- END: Keywords -----     //


//     ----- Identifiers -----     //
StringArray identifiers_table = {.entries = NULL, .size = 0};
typedef struct {
    int id; // the index in the identifiers_table
} Identifier;
//     ----- END: Identifiers -----     //


//     ----- Constants -----     //
typedef enum {
    INTEGER = 0,
    CHARACTER
} token_constant_type;
typedef struct {
    token_constant_type type;
    int raw_value; // inferred based on type.
} Constant;
//     ----- END: Constants -----     //


//     ----- String Literals -----     //
StringArray string_literals_table = {.entries = NULL, .size = 0};
typedef struct {
    int id; // the index in the string_literals_table
} StringLiteral;
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
    TokenCategory category;
    void* data;
} Token;

//     ----- Lexing Logic -----     //
int
//     ----- END: Lexing Logic -----     //

#endif //GIT_CODE_LEXER_H