//
// Created by tomer on 27-Sep-26.
//

#ifndef GIT_CODE_TOKEN_H
#define GIT_CODE_TOKEN_H

#include "Context.h"

// Token Categories
typedef enum {
    TOKEN_CATEGORY_NONE = 0,
    TOKEN_CATEGORY_EOF,
    TOKEN_CATEGORY_KEYWORD,
    TOKEN_CATEGORY_IDENTIFIER,
    TOKEN_CATEGORY_CONSTANT,
    TOKEN_CATEGORY_STRING_LITERAL,
    TOKEN_CATEGORY_PUNCTUATOR
} TokenCategory;
// Keyword
typedef enum {
    TOKEN_KEYWORD_ID_CHAR,
    TOKEN_KEYWORD_ID_ELSE,
    TOKEN_KEYWORD_ID_IF,
    TOKEN_KEYWORD_ID_INT,
    TOKEN_KEYWORD_ID_RETURN,
    TOKEN_KEYWORD_ID_VOID,
    TOKEN_KEYWORD_ID_WHILE,
    TOKEN_KEYWORD_ID__COUNT // sentinel
} TokenKeywordID;
typedef struct {
    TokenKeywordID id;
} TokenKeyword;
// Identifier
typedef struct {
    int id; // the index in the identifiers_table
} TokenIdentifier;
// Constants
typedef enum {
    TOKEN_CONSTANT_TYPE_INTEGER = 0,
    TOKEN_CONSTANT_TYPE_CHARACTER,
    TOKEN_CONSTANT_TYPE__COUNT // sentinel
} TokenConstantType;
typedef struct {
    TokenConstantType type;
    int value; // inferred based on type.
} TokenConstant;
// String Literals
typedef struct {
    int id; // the index in the string_literals_table
} TokenStringLiteral;
// Punctuators
typedef enum {
    TOKEN_PUNCTUATOR_ID_BRACKET_LEFT,       // [
    TOKEN_PUNCTUATOR_ID_BRACKET_RIGHT,      // ]
    TOKEN_PUNCTUATOR_ID_PARENTHESES_LEFT,   // (
    TOKEN_PUNCTUATOR_ID_PARENTHESES_RIGHT,  // )

    TOKEN_PUNCTUATOR_ID_BRACE_LEFT,         // {
    TOKEN_PUNCTUATOR_ID_BRACE_RIGHT,        // }
    TOKEN_PUNCTUATOR_ID_AMPERSAND,
    TOKEN_PUNCTUATOR_ID_STAR,

    TOKEN_PUNCTUATOR_ID_PLUS,
    TOKEN_PUNCTUATOR_ID_MINUS,
    TOKEN_PUNCTUATOR_ID_TILDA,
    TOKEN_PUNCTUATOR_ID_EXCLAMATION_MARK,

    TOKEN_PUNCTUATOR_ID_FORWARD_SLASH,
    TOKEN_PUNCTUATOR_ID_PERCENTAGE,
    TOKEN_PUNCTUATOR_ID_HAT,
    TOKEN_PUNCTUATOR_ID_ASSIGNMENT,

    TOKEN_PUNCTUATOR_ID_LEFT_SHIFT,
    TOKEN_PUNCTUATOR_ID_RIGHT_SHIFT,
    TOKEN_PUNCTUATOR_ID_GREATER,
    TOKEN_PUNCTUATOR_ID_SMALLER,

    TOKEN_PUNCTUATOR_ID_GREATER_EQUALS,
    TOKEN_PUNCTUATOR_ID_SMALLER_EQUALS,
    TOKEN_PUNCTUATOR_ID_EQUALS,
    TOKEN_PUNCTUATOR_ID_NOT_EQUALS,

    TOKEN_PUNCTUATOR_ID_BAR,
    TOKEN_PUNCTUATOR_ID_LOGICAL_AND,
    TOKEN_PUNCTUATOR_ID_LOGICAL_OR,
    TOKEN_PUNCTUATOR_ID_SEMICOLON,

    TOKEN_PUNCTUATOR_ID__COUNT // sentinel
} TokenPunctuatorID;
typedef struct {
    TokenCategory category;
    TokenPunctuatorID id;
} TokenPunctuator;
// Token
typedef struct {
    TokenCategory category;
    union {
        TokenKeyword keyword;
        TokenIdentifier identifier;
        TokenConstant constant;
        TokenStringLiteral string_literal;
        TokenPunctuator punctuator;
    } as;
} Token; // a tagged union

// ---------- token factories ------------- //
static Token token_keyword_create(TokenKeywordID id);
// vvv: 1. string is stack allocated; 2. assuming valid string.
static Token token_identifier_create(const char* string);
static Token token_constant_create(TokenConstantType type, int value);
// vvv: 1. string is stack allocated; 2. assuming valid string.
static Token token_string_literal_create(const char* string);
static Token token_punctuator_create(TokenPunctuatorID id);

// -------- other -------- //
extern const Token TOKEN_EMPTY;
extern const Token TOKEN_EOF;

bool token_is_EOF(Token token);

#endif //GIT_CODE_TOKEN_H