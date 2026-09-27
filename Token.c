//
// Created by tomer on 27-Sep-26.
//

#include "Token.h"

const Token TOKEN_EOF = {.kind = TOKEN_KIND_EOF};

const char* const token_spelling_table[TOKEN_KIND__COUNT] = {
    // Keywords
    [TOKEN_KW_CHAR]             = "char",
    [TOKEN_KW_ELSE]             = "else",
    [TOKEN_KW_IF]               = "if",
    [TOKEN_KW_INT]              = "int",
    [TOKEN_KW_RETURN]           = "return",
    [TOKEN_KW_VOID]             = "void",
    [TOKEN_KW_WHILE]            = "while",
    // Punctuators
    [TOKEN_BRACKET_LEFT]        = "[",
    [TOKEN_BRACKET_RIGHT]       = "]",
    [TOKEN_PARENTHESES_LEFT]    = "(",
    [TOKEN_PARENTHESES_RIGHT]   = ")",
    [TOKEN_BRACE_LEFT]          = "{",
    [TOKEN_BRACE_RIGHT]         = "}",
    [TOKEN_COMMA]               = ",",
    [TOKEN_STAR]                = "*",
    [TOKEN_PLUS]                = "+",
    [TOKEN_MINUS]               = "-",
    [TOKEN_TILDE]               = "~",
    [TOKEN_EXCLAMATION]         = "!",
    [TOKEN_FORWARD_SLASH]       = "/",
    [TOKEN_PERCENTAGE]          = "%",
    [TOKEN_CARET]               = "^",
    [TOKEN_EQUALS]              = "=",
    [TOKEN_LESS_LESS]           = "<<",
    [TOKEN_GREATER_GREATER]     = ">>",
    [TOKEN_GREATER]             = ">",
    [TOKEN_LESS]                = "<",
    [TOKEN_GREATER_EQUALS]      = ">=",
    [TOKEN_LESS_EQUALS]         = "<=",
    [TOKEN_EQUALS_EQUALS]       = "==",
    [TOKEN_EXCLAMATION_EQUALS]  = "!=",
    [TOKEN_BAR]                 = "|",
    [TOKEN_AMPERSAND]           = "&",
    [TOKEN_AMPERSAND_AMPERSAND] = "&&",
    [TOKEN_BAR_BAR]             = "||",
    [TOKEN_SEMICOLON]           = ";"
};

bool token_is_EOF(const Token token) {
    return token.kind == TOKEN_KIND_EOF;
}