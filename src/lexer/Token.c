//
// Created by tomer on 27-Sep-26.
//

#include "Token.h"

#include <assert.h>

const TextView token_kind_spelling_table[TOKEN__SPELLABLE_COUNT] = {
    // Keywords
    [TOKEN_KW_CHAR]             = text_view_from_literal("char"),
    [TOKEN_KW_INT]              = text_view_from_literal("int"),
    [TOKEN_KW_VOID]             = text_view_from_literal("void"),
    [TOKEN_KW_WHILE]            = text_view_from_literal("while"),
    [TOKEN_KW_IF]               = text_view_from_literal("if"),
    [TOKEN_KW_ELSE]             = text_view_from_literal("else"),
    [TOKEN_KW_RETURN]           = text_view_from_literal("return"),
    // Punctuators
    [TOKEN_BRACKET_LEFT]        = text_view_from_literal("["),
    [TOKEN_BRACKET_RIGHT]       = text_view_from_literal("]"),
    [TOKEN_PARENTHESES_LEFT]    = text_view_from_literal("("),
    [TOKEN_PARENTHESES_RIGHT]   = text_view_from_literal(")"),
    [TOKEN_BRACE_LEFT]          = text_view_from_literal("{"),
    [TOKEN_BRACE_RIGHT]         = text_view_from_literal("}"),
    [TOKEN_COMMA]               = text_view_from_literal(","),
    [TOKEN_STAR]                = text_view_from_literal("*"),
    [TOKEN_PLUS]                = text_view_from_literal("+"),
    [TOKEN_MINUS]               = text_view_from_literal("-"),
    [TOKEN_TILDE]               = text_view_from_literal("~"),
    [TOKEN_EXCLAMATION]         = text_view_from_literal("!"),
    [TOKEN_FORWARD_SLASH]       = text_view_from_literal("/"),
    [TOKEN_PERCENT]          = text_view_from_literal("%"),
    [TOKEN_CARET]               = text_view_from_literal("^"),
    [TOKEN_EQUALS]              = text_view_from_literal("="),
    [TOKEN_LESS_LESS]           = text_view_from_literal("<<"),
    [TOKEN_GREATER_GREATER]     = text_view_from_literal(">>"),
    [TOKEN_GREATER]             = text_view_from_literal(">"),
    [TOKEN_LESS]                = text_view_from_literal("<"),
    [TOKEN_GREATER_EQUALS]      = text_view_from_literal(">="),
    [TOKEN_LESS_EQUALS]         = text_view_from_literal("<="),
    [TOKEN_EQUALS_EQUALS]       = text_view_from_literal("=="),
    [TOKEN_EXCLAMATION_EQUALS]  = text_view_from_literal("!="),
    [TOKEN_BAR]                 = text_view_from_literal("|"),
    [TOKEN_AMPERSAND]           = text_view_from_literal("&"),
    [TOKEN_AMPERSAND_AMPERSAND] = text_view_from_literal("&&"),
    [TOKEN_BAR_BAR]             = text_view_from_literal("||"),
    [TOKEN_SEMICOLON]           = text_view_from_literal(";")
};

TextView token_kind_get_spelling(const TokenKind kind) {
    assert(0 <= kind && kind < TOKEN__SPELLABLE_COUNT);
    return token_kind_spelling_table[kind];
}
TokenKind token_kind_from_spelling(const TextView spelling) {
    for (TokenKind kind = TOKEN__SPELLABLE_FIRST; kind <= TOKEN__SPELLABLE_LAST; ++kind)
        if (text_view_equals(spelling, token_kind_spelling_table[kind]))
            return kind;
    return TOKEN_NONE;
}
bool token_kind_is_keyword(const TokenKind kind) {
    return TOKEN__KW_FIRST <= kind && kind <= TOKEN__KW_LAST;
}
bool token_kind_is_punctuator(const TokenKind kind) {
    return TOKEN__PUNC_FIRST <= kind && kind <= TOKEN__PUNC_LAST;
}