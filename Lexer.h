//
// Created by tomer on 22-Sep-26.
//

#ifndef GIT_CODE_LEXER_H
#define GIT_CODE_LEXER_H

#include <stdlib.h>
#include "DataStructures.h"

// -------- Token Categories -------- //
typedef enum {
    KEYWORD = 0,
    IDENTIFIER,
    CONSTANT,
    STRING_LITERAL,
    PUNCTUATORS
} TokenCategory;
// -------- END: Token Categories -------- //


// -------- Keywords -------- //
typedef enum {
    CHAR,
    ELSE,
    IF,
    INT,
    RETURN,
    VOID,
    WHILE
} TokenKeywordID;
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
    TokenKeywordID id;
} TokenKeyword;
// -------- END: Keywords -------- //


// -------- Identifiers -------- //
StringArray* identifiers_table = NULL; // populated at runtime.
typedef struct {
    int id; // the index in the identifiers_table
} TokenIdentifier;
// -------- END: Identifiers -------- //


// -------- Constants -------- //
typedef enum {
    INTEGER = 0,
    CHARACTER
} token_constant_type;
typedef struct {
    token_constant_type type;
    int raw_value; // inferred based on type.
} TokenConstant;
// -------- END: Constants -------- //


// -------- String Literals -------- //
StringArray* string_literals_table = NULL; // populated at runtime.
typedef struct {
    int id; // the index in the string_literals_table
} TokenStringLiteral;
// -------- END: String Literals -------- //


// -------- Punctuators -------- //
typedef enum {
    TOKEN_PUNCTUATOR_BRACKET_LEFT,       // [
    TOKEN_PUNCTUATOR_BRACKET_RIGHT,      // ]
    TOKEN_PUNCTUATOR_PARENTHESES_LEFT,   // (
    TOKEN_PUNCTUATOR_PARENTHESES_RIGHT,  // )

    TOKEN_PUNCTUATOR_BRACE_LEFT,         // {
    TOKEN_PUNCTUATOR_BRACE_RIGHT,        // }
    TOKEN_PUNCTUATOR_AMPERSAND,
    TOKEN_PUNCTUATOR_STAR,

    TOKEN_PUNCTUATOR_PLUS,
    TOKEN_PUNCTUATOR_MINUS,
    TOKEN_PUNCTUATOR_TILDA,
    TOKEN_PUNCTUATOR_EXCLAMATION_MARK,

    TOKEN_PUNCTUATOR_FORWARD_SLASH,
    TOKEN_PUNCTUATOR_PERCENTAGE,
    TOKEN_PUNCTUATOR_HAT,
    TOKEN_PUNCTUATOR_ASSIGNMENT,

    TOKEN_PUNCTUATOR_LEFT_SHIFT,
    TOKEN_PUNCTUATOR_RIGHT_SHIFT,
    TOKEN_PUNCTUATOR_GREATER,
    TOKEN_PUNCTUATOR_SMALLER,

    TOKEN_PUNCTUATOR_GREATER_EQUALS,
    TOKEN_PUNCTUATOR_SMALLER_EQUALS,
    TOKEN_PUNCTUATOR_EQUALS,
    TOKEN_PUNCTUATOR_NOT_EQUALS,

    TOKEN_PUNCTUATOR_BAR,
    TOKEN_PUNCTUATOR_LOGICAL_AND,
    TOKEN_PUNCTUATOR_LOGICAL_OR,
    TOKEN_PUNCTUATOR_SEMICOLON
} TokenPunctuatorID;
const char* punctuators_table[] = {
    "[", "]", "(", ")",
    "{", "}", "&", "*",
    "+", "-", "~", "!",
    "/", "%", "^", "=",
    "<<", ">>", ">", "<",
    ">=", "<=", "==", "!=",
    "|", "&&", "||", ";"
};
typedef struct {
    TokenPunctuatorID id;
} TokenPunctuator;
// -------- END: Punctuators -------- //

typedef struct {
    TokenCategory category;
    void* data;
} Token;

// -------- Lexing Logic -------- //

typedef struct {
    int SPI; // SPI: Stream Position Indicator. i.e., the index of the next char to parse.
    char* data;
} LexerStream;

/**
 * Create a new LexerStream struct.
 * @param data the lexeme that the lexer should parse
 * @return a LexerStream handle
 */
LexerStream* lexer_stream_create(const char* data);

/**
 * Destroys the LexerStream handle
 * @param handle
 */
void lexer_stream_destroy(LexerStream* handle);

/**
 * Peeks the current char of the lexeme.
 * @param handle
 */
char lexer_stream_peek(const LexerStream* handle);

/**
 * Retrieves the current byte of the lexeme, then advances SPI.
 * @param handle
 * @return the current byte of the lexeme.
 */
char lexer_stream_pop(LexerStream* handle);

/**
 * extract the next token based on the lexeme.
 * @param handle
 * @return the next token based on the lexeme.
 */
Token* lexer_stream_extract_token(LexerStream* handle);

// -------- END: Lexing Logic -------- //

#endif //GIT_CODE_LEXER_H