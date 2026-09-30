//
// Created by tomer on 27-Sep-26.
//

#include "Token.h"

const StringView token_kind_spelling_table[TOKEN__SPELLABLE_COUNT] = {
    // Keywords
    [TOKEN_KW_CHAR]             = string_view_from_literal("char"),
    [TOKEN_KW_INT]              = string_view_from_literal("int"),
    [TOKEN_KW_VOID]             = string_view_from_literal("void"),
    [TOKEN_KW_WHILE]            = string_view_from_literal("while"),
    [TOKEN_KW_IF]               = string_view_from_literal("if"),
    [TOKEN_KW_ELSE]             = string_view_from_literal("else"),
    [TOKEN_KW_RETURN]           = string_view_from_literal("return"),
    // Punctuators
    [TOKEN_BRACKET_LEFT]        = string_view_from_literal("["),
    [TOKEN_BRACKET_RIGHT]       = string_view_from_literal("]"),
    [TOKEN_PARENTHESES_LEFT]    = string_view_from_literal("("),
    [TOKEN_PARENTHESES_RIGHT]   = string_view_from_literal(")"),
    [TOKEN_BRACE_LEFT]          = string_view_from_literal("{"),
    [TOKEN_BRACE_RIGHT]         = string_view_from_literal("}"),
    [TOKEN_COMMA]               = string_view_from_literal(","),
    [TOKEN_STAR]                = string_view_from_literal("*"),
    [TOKEN_PLUS]                = string_view_from_literal("+"),
    [TOKEN_MINUS]               = string_view_from_literal("-"),
    [TOKEN_TILDE]               = string_view_from_literal("~"),
    [TOKEN_EXCLAMATION]         = string_view_from_literal("!"),
    [TOKEN_FORWARD_SLASH]       = string_view_from_literal("/"),
    [TOKEN_PERCENTAGE]          = string_view_from_literal("%"),
    [TOKEN_CARET]               = string_view_from_literal("^"),
    [TOKEN_EQUALS]              = string_view_from_literal("="),
    [TOKEN_LESS_LESS]           = string_view_from_literal("<<"),
    [TOKEN_GREATER_GREATER]     = string_view_from_literal(">>"),
    [TOKEN_GREATER]             = string_view_from_literal(">"),
    [TOKEN_LESS]                = string_view_from_literal("<"),
    [TOKEN_GREATER_EQUALS]      = string_view_from_literal(">="),
    [TOKEN_LESS_EQUALS]         = string_view_from_literal("<="),
    [TOKEN_EQUALS_EQUALS]       = string_view_from_literal("=="),
    [TOKEN_EXCLAMATION_EQUALS]  = string_view_from_literal("!="),
    [TOKEN_BAR]                 = string_view_from_literal("|"),
    [TOKEN_AMPERSAND]           = string_view_from_literal("&"),
    [TOKEN_AMPERSAND_AMPERSAND] = string_view_from_literal("&&"),
    [TOKEN_BAR_BAR]             = string_view_from_literal("||"),
    [TOKEN_SEMICOLON]           = string_view_from_literal(";")
};

StringView token_kind_get_spelling(const TokenKind kind) {
    assert(0 <= kind && kind < TOKEN__SPELLABLE_COUNT);
    return token_kind_spelling_table[kind];
}
TokenKind token_kind_from_spelling(const StringView spelling) {
    for (TokenKind kind = TOKEN__SPELLABLE_FIRST; kind <= TOKEN__SPELLABLE_LAST; ++kind)
        if (string_view_equals(spelling, token_kind_spelling_table[kind]))
            return kind;
    return TOKEN_KIND_NONE;
}

bool token_is_EOF(const Token token) {
    return token.kind == TOKEN_KIND_EOF;
}
bool token_kind_is_keyword(const TokenKind kind) {
    return TOKEN__KW_FIRST <= kind && kind <= TOKEN__KW_LAST;
}
bool token_kind_is_punctuator(const TokenKind kind) {
    return TOKEN__PUNC_FIRST <= kind && kind <= TOKEN__PUNC_LAST;
}