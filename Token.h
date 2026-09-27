//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_TOKEN_H
#define C_COMPILER_TOKEN_H

#include <stdbool.h>

#include "Range.h"
#include "List.h"

// Token Categories
typedef enum {
    // Specials
    TOKEN_KIND_NONE = 0,
    TOKEN_KIND_EOF,
    // Keywords
    TOKEN_KW_CHAR,
    TOKEN_KW_ELSE,
    TOKEN_KW_IF,
    TOKEN_KW_INT,
    TOKEN_KW_RETURN,
    TOKEN_KW_VOID,
    TOKEN_KW_WHILE,
    // Identifiers
    TOKEN_IDENT,
    // Constants
    TOKEN_CONST_INT,
    // String Literals
    TOKEN_STRING_LITERAL,
    // Punctuators
    TOKEN_BRACKET_LEFT,         // [
    TOKEN_BRACKET_RIGHT,        // ]
    TOKEN_PARENTHESES_LEFT,     // (
    TOKEN_PARENTHESES_RIGHT,    // )
    // ------------------------ //
    TOKEN_BRACE_LEFT,           // {
    TOKEN_BRACE_RIGHT,          // }
    TOKEN_COMMA,                // ,
    TOKEN_STAR,                 // *
    // ------------------------ //
    TOKEN_PLUS,                 // +
    TOKEN_MINUS,                // -
    TOKEN_TILDE,                // ~
    TOKEN_EXCLAMATION,          // !
    // ------------------------ //
    TOKEN_FORWARD_SLASH,        // /
    TOKEN_PERCENTAGE,           // %
    TOKEN_CARET,                // ^
    TOKEN_EQUALS,               // =
    // ------------------------ //
    TOKEN_LESS_LESS,            // <<
    TOKEN_GREATER_GREATER,      // >>
    TOKEN_GREATER,              // >
    TOKEN_LESS,                 // <
    // ------------------------ //
    TOKEN_GREATER_EQUALS,       // >=
    TOKEN_LESS_EQUALS,          // <=
    TOKEN_EQUALS_EQUALS,        // ==
    TOKEN_EXCLAMATION_EQUALS,   // !=
    // ------------------------ //
    TOKEN_BAR,                  // |
    TOKEN_AMPERSAND,            // &
    TOKEN_AMPERSAND_AMPERSAND,  // &&
    TOKEN_BAR_BAR,              // ||
    // ------------------------ //
    TOKEN_SEMICOLON,            // ;
    // Sentinel                 //
    TOKEN_KIND__COUNT           //
} TokenKind;
// Token
typedef struct {
    TokenKind kind;
    Range spelling; ///< the part of the @c SourceFile (lexeme) that is processed as the @c Token.
    union {
        int id;
        int value;
    } as; ///< a tagged @c union, encapsulating the different @c TokenKind 's fields.
} Token; ///< tagged union

extern const Token TOKEN_EOF;

bool token_is_EOF(Token token);

#endif //C_COMPILER_TOKEN_H