//
// Created by tomer on 27-Sep-26.
//

#include "Token.h"

#include <assert.h>

const TextView token_kind_spelling_table[TOKEN__SPELLABLE_COUNT] = {
    // Keywords
    [TOKEN_KW_CHAR]             = TEXT_VIEW_INITIALIZER_OF_LITERAL("char"),
    [TOKEN_KW_INT]              = TEXT_VIEW_INITIALIZER_OF_LITERAL("int"),
    [TOKEN_KW_VOID]             = TEXT_VIEW_INITIALIZER_OF_LITERAL("void"),
    [TOKEN_KW_WHILE]            = TEXT_VIEW_INITIALIZER_OF_LITERAL("while"),
    [TOKEN_KW_IF]               = TEXT_VIEW_INITIALIZER_OF_LITERAL("if"),
    [TOKEN_KW_ELSE]             = TEXT_VIEW_INITIALIZER_OF_LITERAL("else"),
    [TOKEN_KW_RETURN]           = TEXT_VIEW_INITIALIZER_OF_LITERAL("return"),
    // Punctuators
    [TOKEN_BRACKET_LEFT]        = TEXT_VIEW_INITIALIZER_OF_LITERAL("["),
    [TOKEN_BRACKET_RIGHT]       = TEXT_VIEW_INITIALIZER_OF_LITERAL("]"),
    [TOKEN_PARENTHESES_LEFT]    = TEXT_VIEW_INITIALIZER_OF_LITERAL("("),
    [TOKEN_PARENTHESES_RIGHT]   = TEXT_VIEW_INITIALIZER_OF_LITERAL(")"),
    [TOKEN_BRACE_LEFT]          = TEXT_VIEW_INITIALIZER_OF_LITERAL("{"),
    [TOKEN_BRACE_RIGHT]         = TEXT_VIEW_INITIALIZER_OF_LITERAL("}"),
    [TOKEN_COMMA]               = TEXT_VIEW_INITIALIZER_OF_LITERAL(","),
    [TOKEN_STAR]                = TEXT_VIEW_INITIALIZER_OF_LITERAL("*"),
    [TOKEN_PLUS]                = TEXT_VIEW_INITIALIZER_OF_LITERAL("+"),
    [TOKEN_MINUS]               = TEXT_VIEW_INITIALIZER_OF_LITERAL("-"),
    [TOKEN_TILDE]               = TEXT_VIEW_INITIALIZER_OF_LITERAL("~"),
    [TOKEN_EXCLAMATION]         = TEXT_VIEW_INITIALIZER_OF_LITERAL("!"),
    [TOKEN_FORWARD_SLASH]       = TEXT_VIEW_INITIALIZER_OF_LITERAL("/"),
    [TOKEN_PERCENT]             = TEXT_VIEW_INITIALIZER_OF_LITERAL("%"),
    [TOKEN_CARET]               = TEXT_VIEW_INITIALIZER_OF_LITERAL("^"),
    [TOKEN_EQUALS]              = TEXT_VIEW_INITIALIZER_OF_LITERAL("="),
    [TOKEN_LESS_LESS]           = TEXT_VIEW_INITIALIZER_OF_LITERAL("<<"),
    [TOKEN_GREATER_GREATER]     = TEXT_VIEW_INITIALIZER_OF_LITERAL(">>"),
    [TOKEN_GREATER]             = TEXT_VIEW_INITIALIZER_OF_LITERAL(">"),
    [TOKEN_LESS]                = TEXT_VIEW_INITIALIZER_OF_LITERAL("<"),
    [TOKEN_GREATER_EQUALS]      = TEXT_VIEW_INITIALIZER_OF_LITERAL(">="),
    [TOKEN_LESS_EQUALS]         = TEXT_VIEW_INITIALIZER_OF_LITERAL("<="),
    [TOKEN_EQUALS_EQUALS]       = TEXT_VIEW_INITIALIZER_OF_LITERAL("=="),
    [TOKEN_EXCLAMATION_EQUALS]  = TEXT_VIEW_INITIALIZER_OF_LITERAL("!="),
    [TOKEN_BAR]                 = TEXT_VIEW_INITIALIZER_OF_LITERAL("|"),
    [TOKEN_AMPERSAND]           = TEXT_VIEW_INITIALIZER_OF_LITERAL("&"),
    [TOKEN_AMPERSAND_AMPERSAND] = TEXT_VIEW_INITIALIZER_OF_LITERAL("&&"),
    [TOKEN_BAR_BAR]             = TEXT_VIEW_INITIALIZER_OF_LITERAL("||"),
    [TOKEN_SEMICOLON]           = TEXT_VIEW_INITIALIZER_OF_LITERAL(";")
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
bool token_kind_is_type(const TokenKind kind) {
    return kind == TOKEN_KW_VOID || kind == TOKEN_KW_CHAR || kind == TOKEN_KW_INT;
}
TypeKind token_kind_get_type_kind(const TokenKind kind) {
    assert(token_kind_is_type(kind));

    if (kind == TOKEN_KW_VOID)
        return TYPE_KIND_VOID;
    if (kind == TOKEN_KW_CHAR)
        return TYPE_KIND_CHAR;
    if (kind == TOKEN_KW_INT)
        return TYPE_KIND_INT;
    assert(false && "invalid program state"); // should have been caught in the previous assert
}