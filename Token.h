//
// Created by tomer on 27-Sep-26.
//

#ifndef C_COMPILER_TOKEN_H
#define C_COMPILER_TOKEN_H

#include <stdbool.h>

#include "Range.h"
#include "StringView.h"

// Token Kinds. WHEN ADDING A NEW ONE, DO NOT FORGOT TO UPDATE THE ALIASES AT THE END!!
typedef enum {
    // First, all spellable token kinds:
    // Keywords
    TOKEN_KW_CHAR = 0,          // char
    TOKEN_KW_INT,               // int
    TOKEN_KW_VOID,              // void
    TOKEN_KW_WHILE,             // while
    TOKEN_KW_IF,                // if
    TOKEN_KW_ELSE,              // else
    TOKEN_KW_RETURN,            // return
    // Punctuators              //
    TOKEN_BRACKET_LEFT,         // [
    TOKEN_BRACKET_RIGHT,        // ]
    TOKEN_PARENTHESES_LEFT,     // (
    TOKEN_PARENTHESES_RIGHT,    // )
    TOKEN_BRACE_LEFT,           // {
    TOKEN_BRACE_RIGHT,          // }
    TOKEN_COMMA,                // ,
    TOKEN_STAR,                 // *
    TOKEN_PLUS,                 // +
    TOKEN_MINUS,                // -
    TOKEN_TILDE,                // ~
    TOKEN_EXCLAMATION,          // !
    TOKEN_FORWARD_SLASH,        // /
    TOKEN_PERCENTAGE,           // %
    TOKEN_CARET,                // ^
    TOKEN_EQUALS,               // =
    TOKEN_LESS_LESS,            // <<
    TOKEN_GREATER_GREATER,      // >>
    TOKEN_GREATER,              // >
    TOKEN_LESS,                 // <
    TOKEN_GREATER_EQUALS,       // >=
    TOKEN_LESS_EQUALS,          // <=
    TOKEN_EQUALS_EQUALS,        // ==
    TOKEN_EXCLAMATION_EQUALS,   // !=
    TOKEN_BAR,                  // |
    TOKEN_AMPERSAND,            // &
    TOKEN_AMPERSAND_AMPERSAND,  // &&
    TOKEN_BAR_BAR,              // ||
    TOKEN_SEMICOLON,            // ;
    // sentinel                 //
    TOKEN__SPELLABLE_COUNT,
    // Then non-spellable token kinds:
    // Identifiers
    TOKEN_IDENT,
    // Constants
    TOKEN_CONST_INT,
    // String Literals
    TOKEN_STRING_LITERAL,
    // Specials
    TOKEN_KIND_NONE,
    TOKEN_KIND_UNKNOWN,
    TOKEN_KIND_EOF,
    // Aliases
    TOKEN__KW_FIRST = TOKEN_KW_CHAR,
    TOKEN__KW_LAST = TOKEN_KW_RETURN,

    TOKEN__PUNC_FIRST = TOKEN_BRACKET_LEFT,
    TOKEN__PUNC_LAST = TOKEN_SEMICOLON,

    TOKEN__SPELLABLE_FIRST = TOKEN__KW_FIRST,
    TOKEN__SPELLABLE_LAST = TOKEN__PUNC_LAST
} TokenKind;
// Token
typedef struct {
    TokenKind kind;
    Range range; ///< the part of the @c SourceFile (lexeme) that is processed as the @c Token.
    union {
        int id; ///< for identifiers, string literals
        int value; ///< for constants
    } as; ///< a tagged @c union, encapsulating the different @c TokenKind 's fields.
} Token; ///< tagged union

extern const StringView token_kind_spelling_table[TOKEN__SPELLABLE_COUNT];

StringView token_kind_get_spelling(TokenKind kind);
TokenKind token_kind_from_spelling(StringView spelling);

bool token_is_EOF(Token token);
bool token_kind_is_keyword(TokenKind kind);
bool token_kind_is_punctuator(TokenKind kind);

#endif //C_COMPILER_TOKEN_H