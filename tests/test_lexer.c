//
// Written by tomer on 02-Oct-26.
//

#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Harness.h"
#include "Lexer.h"
#include "Token.h"
#include "Utils.h"
#include "Text.h"
#include "TextView.h"

/**
 * returns the @c Token @c List which was lexed from @b buffer.
 * @attention ownership of @b buffer remains with the caller.
 */
static List* test_lexer_lex_buffer(const char* const buffer, const int length) {
    Context* context = context_create(source_text_create(text_create_from_buffer(buffer, length), "<source>"));
    Lexer* lexer = lexer_create(context, 0);
    List* tokens = lexer_lex_source_text(lexer);
    lexer_destroy(lexer);
    context_destroy(context);
    return tokens;
}
static bool are_tokens_equal(const Token* first, const Token* second, const Context* const context_first, const Context* const context_second) {
    assert(first != NULL && second != NULL);
    assert(first->kind != TOKEN_UNKNOWN && second->kind != TOKEN_UNKNOWN);

    if (!range_equals(first->range, second->range))
        return false;   /* note: range_equals assumes the underling Context-s are the same.
                         * They are not in this case. However, it is valid since range_equals
                         * only checks the position offset and length, and this is what we
                         * want to find out.
                         */
    if (token_is_eof(*first) && token_is_eof(*second))
        return true;
    if (first->kind != second->kind)
        return false;

    const TokenKind kind = first->kind; // could've been second->kind as well.
    if (kind == TOKEN_CONST_INT)
        return first->as.value == second->as.value;
    if (kind == TOKEN_IDENT) {
        const TextView* first_spelling = list_at_const(context_first->identifiers_table, first->as.id);
        const TextView* second_spelling = list_at_const(context_second->identifiers_table, second->as.id);
        return text_view_equals(*first_spelling, *second_spelling);
    }
    if (kind == TOKEN_STRING_LITERAL) {
        // TODO: potentially modify once string literal manufacturing is updated.
        return text_list_compare(
            list_at_const(context_first->string_literals_table, first->as.id),
            list_at_const(context_second->string_literals_table, second->as.id)
            ) == 0;
    }
    if (token_kind_is_keyword(kind) || token_kind_is_punctuator(kind)) {
        return true; // because all other conditions has already been verified.
    }

    assert(false && "Unknown token kind");
    return false; // must return some value, should not get here.
}
static bool are_token_lists_equal(
    const List* const first,
    const List* const second,
    const Context* const context_first,
    const Context* const context_second) {
    if (list_get_size(first) != list_get_size(second))
        return false;
    const int N = list_get_size(first);
    for (int i = 0; i < N; ++i)
        if (!are_tokens_equal(
            list_at_const(first, i),
            list_at_const(second, i),
            context_first,
            context_second))
            return false;
    return true;
}
static Token test_token_punc_keyword_create(
    const int range__source_text_index,
    const int range__offset,
    const int range__length,
    const TokenKind kind) {
    return (Token){
        .kind = kind,
        .range = {
            .position = {
                .source_text_index = range__source_text_index,
                .offset = range__offset
            },
            .length = range__length
        }
    };
}
static Token test_token_const_int_create(
    const int range__source_text_index,
    const int range__offset,
    const int range__length,
    const int value) {
    return (Token){
        .kind = TOKEN_CONST_INT,
        .range = {
            .position = {
                .source_text_index = range__source_text_index,
                .offset = range__offset
            },
            .length = range__length
        },
        .as.value = value
    };
}
static Token test_token_ident_create(
    const int range__source_text_index,
    const int range__offset,
    const int range__length,
    Context* const context,
    const TextView spelling) {
    return (Token){
        .kind = TOKEN_IDENT,
        .range = {
            .position = {
                .source_text_index = range__source_text_index,
                .offset = range__offset
            },
            .length = range__length
        },
        .as.id = list_find_or_push(context->identifiers_table, &spelling, text_view_list_compare, text_view_list_duplicate)
    };
}
static Token test_token_string_literal_create(
    const int range__source_text_index,
    const int range__offset,
    const int range__length,
    Context* const context,
    const TextView text_view) {
    Text* text = text_create_from_view(text_view);
    const Token to_return = (Token){
        .kind = TOKEN_STRING_LITERAL,
        .range = {
            .position = {
                .source_text_index = range__source_text_index,
                .offset = range__offset
            },
            .length = range__length
        },
        .as.id = list_find_or_push(context->string_literals_table, text, text_list_compare, text_list_duplicate)
    };
    text_destroy(text);
    return to_return;
}
static Token* test_token_create_ptr(Token token) {
    Token* to_return = utils_xmalloc(sizeof (Token));
    *to_return = token;
    return to_return;
}

static void test_lexer_empty_buffer(void) {
    List* tokens = test_lexer_lex_buffer("", 0);

    CHECK(list_get_size(tokens) == 1);
    CHECK(token_is_eof(*(Token*)list_at(tokens, 0)));

    list_destroy(tokens);
}
static void test_lexer_single_punctuator(void) {
    for (TokenKind kind = TOKEN__PUNC_FIRST; kind <= TOKEN__PUNC_LAST; ++kind) {
        const TextView view = token_kind_spelling_table[kind];
        List* tokens = test_lexer_lex_buffer(view.data, view.length);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == kind);
        CHECK(token_is_eof(*second));

        list_destroy(tokens);
    }
}
static void test_lexer_single_keyword(void) {
    for (TokenKind kind = TOKEN__KW_FIRST; kind <= TOKEN__KW_LAST; ++kind) {
        const TextView view = token_kind_spelling_table[kind];
        List* tokens = test_lexer_lex_buffer(view.data, view.length);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == kind);
        CHECK(token_is_eof(*second));

        list_destroy(tokens);
    }
}
static void test_lexer_single_numeric(void) {
    typedef struct {
        TextView spelling;
        int number;
    } test_numeric;
    test_numeric arr[] = {
        {.spelling = text_view_from_literal("0"), .number = 0},
        {.spelling = text_view_from_literal("1"), .number = 1},
        {.spelling = text_view_from_literal("2"), .number = 2},
        {.spelling = text_view_from_literal("3"), .number = 3},
        {.spelling = text_view_from_literal("4"), .number = 4},
        {.spelling = text_view_from_literal("5"), .number = 5},
        {.spelling = text_view_from_literal("6"), .number = 6},
        {.spelling = text_view_from_literal("7"), .number = 7},
        {.spelling = text_view_from_literal("8"), .number = 8},
        {.spelling = text_view_from_literal("9"), .number = 9},
        {.spelling = text_view_from_literal("10"), .number = 10},
        {.spelling = text_view_from_literal("2147483647"), .number = 2147483647},
        {.spelling = text_view_from_literal("4953728"), .number = 4953728},
        {.spelling = text_view_from_literal("111111111"), .number = 111111111},
        {.spelling = text_view_from_literal("245800002"), .number = 245800002}
    };
    const int N = sizeof(arr) / sizeof(test_numeric);

    for (int i = 0; i < N; ++i) {
        const test_numeric test = arr[i];
        List* tokens = test_lexer_lex_buffer(test.spelling.data, test.spelling.length);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == TOKEN_CONST_INT);
        CHECK(first->as.value == test.number);
        CHECK(token_is_eof(*second));

        list_destroy(tokens);
    }
}
static void test_lexer_single_char(void) {
    typedef struct {
        TextView spelling;
        int number;
    } test_char;
    test_char arr[] = {
        {.spelling = text_view_from_literal("'0'"), .number = '0'},
        {.spelling = text_view_from_literal("'1'"), .number = '1'},
        {.spelling = text_view_from_literal("'2'"), .number = '2'},
        {.spelling = text_view_from_literal("'3'"), .number = '3'},
        {.spelling = text_view_from_literal("'4'"), .number = '4'},
        {.spelling = text_view_from_literal("'5'"), .number = '5'},
        {.spelling = text_view_from_literal("'6'"), .number = '6'},
        {.spelling = text_view_from_literal("'7'"), .number = '7'},
        {.spelling = text_view_from_literal("'8'"), .number = '8'},
        {.spelling = text_view_from_literal("'9'"), .number = '9'},
        {.spelling = text_view_from_literal("'\\0'"), .number = '\0'},
        {.spelling = text_view_from_literal("'\\n'"), .number = '\n'},
        {.spelling = text_view_from_literal("'\\r'"), .number = '\r'},
        {.spelling = text_view_from_literal("'\\f'"), .number = '\f'},
        {.spelling = text_view_from_literal("'\\a'"), .number = '\a'},
        {.spelling = text_view_from_literal("'\\t'"), .number = '\t'},
    };
    const int N = sizeof(arr) / sizeof(test_char);

    for (int i = 0; i < N; ++i) {
        const test_char test = arr[i];
        List* tokens = test_lexer_lex_buffer(test.spelling.data, test.spelling.length);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == TOKEN_CONST_INT);
        CHECK(first->as.value == test.number);
        CHECK(token_is_eof(*second));

        list_destroy(tokens);
    }
}
static void test_lexer_single_string(void) {
    typedef struct {
        TextView source;
        TextView check_against;
    } test_string;
    test_string arr[] = {
        {.source = text_view_from_literal("\"\""),                                                  .check_against = text_view_from_literal("")},
        {.source = text_view_from_literal("\"'\""),                                                 .check_against = text_view_from_literal("'")},
        {.source = text_view_from_literal("\"''\""),                                                .check_against = text_view_from_literal("''")},
        {.source = text_view_from_literal("\"'''\""),                                               .check_against = text_view_from_literal("'''")},
        {.source = text_view_from_literal("\"        \""),                                          .check_against = text_view_from_literal("        ")},
        {.source = text_view_from_literal("\"\\0\""),                                               .check_against = text_view_from_literal("\0")},
        {.source = text_view_from_literal("\"a\""),                                                 .check_against = text_view_from_literal("a")},
        {.source = text_view_from_literal("\"ab\""),                                                .check_against = text_view_from_literal("ab")},
        {.source = text_view_from_literal("\"abc\""),                                               .check_against = text_view_from_literal("abc")},
        {.source = text_view_from_literal("\"a51fdg2g78\""),                                        .check_against = text_view_from_literal("a51fdg2g78")},
        {.source = text_view_from_literal("\"]\""),                                                 .check_against = text_view_from_literal("]")},
        {.source = text_view_from_literal("\"]%S&@BVBSH@*(SG@*(S*#^@\""),                           .check_against = text_view_from_literal("]%S&@BVBSH@*(SG@*(S*#^@")},
        {.source = text_view_from_literal("\"]%S&@\\nBVBSH@\\n*(SG\\r\\f\\n@*(S*#^@\""),            .check_against = text_view_from_literal("]%S&@\nBVBSH@\n*(SG\r\f\n@*(S*#^@")},
        {.source = text_view_from_literal("\"\\0]%S&@\\nBVB\\0SH@\\n*(SG\\r\\f\\n@*(S*#^@\\0\""),   .check_against = text_view_from_literal("\0]%S&@\nBVB\0SH@\n*(SG\r\f\n@*(S*#^@\0")},
    };
    const int N = sizeof(arr) / sizeof(test_string);

    for (int i = 0; i < N; ++i) {
        const test_string test = arr[i];

        Context* context = context_create(source_text_create(text_create_from_buffer(test.source.data, test.source.length), "<source>"));
        Lexer* lexer = lexer_create(context, 0);
        List* tokens = lexer_lex_source_text(lexer);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == TOKEN_STRING_LITERAL);
        Text* string_literal_text = text_create_from_view(test.check_against);
        CHECK(list_contains(context->string_literals_table, string_literal_text, text_view_list_compare));
        text_destroy(string_literal_text);
        CHECK(token_is_eof(*second));

        list_destroy(tokens);
        lexer_destroy(lexer);
        context_destroy(context);
    }
}
static void test_lexer_single_identifier(void) {
    typedef struct {
        TextView spelling;
        TextView identifier;
    } test_identifier;
    test_identifier arr[] = {
        {.spelling = text_view_from_literal("_"), .identifier = text_view_from_literal("_")},
        {.spelling = text_view_from_literal("a"), .identifier = text_view_from_literal("a")},
        {.spelling = text_view_from_literal("abcdefg"), .identifier = text_view_from_literal("abcdefg")},
        {.spelling = text_view_from_literal("int_but_not_quite"), .identifier = text_view_from_literal("int_but_not_quite")},
        {.spelling = text_view_from_literal("is_char_or_is"), .identifier = text_view_from_literal("is_char_or_is")},
        {.spelling = text_view_from_literal("not_if_is_but_else"), .identifier = text_view_from_literal("not_if_is_but_else")},
        {.spelling = text_view_from_literal("a1_1234"), .identifier = text_view_from_literal("a1_1234")},
        {.spelling = text_view_from_literal("_122345678"), .identifier = text_view_from_literal("_122345678")},
    };
    const int N = sizeof(arr) / sizeof(test_identifier);

    for (int i = 0; i < N; ++i) {
        const test_identifier test = arr[i];

        Context* context = context_create(source_text_create(text_create_from_buffer(test.spelling.data, test.spelling.length), "<source>"));
        Lexer* lexer = lexer_create(context, 0);
        List* tokens = lexer_lex_source_text(lexer);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == TOKEN_IDENT);

        const TextView token_identifier_spelling = *(TextView*)list_at(context->identifiers_table, first->as.id);
        CHECK(text_view_equals(test.identifier, token_identifier_spelling));
        CHECK(token_is_eof(*second));

        list_destroy(tokens);
        lexer_destroy(lexer);
        context_destroy(context);
    }
}
// full lexer tests:
static void test_lexer_int_x_eq_5_sc(void) {
    const TextView source = text_view_from_literal("int x = 5;");
    Context* const lexer_context = context_create(source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    { // builds the list
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 0, 3, TOKEN_KW_INT)));
        list_push(expected_tokens, test_token_create_ptr(test_token_ident_create(       0, 4, 1, expected_context, text_view_from_literal("x"))));
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 6, 1, TOKEN_EQUALS)));
        list_push(expected_tokens, test_token_create_ptr(test_token_const_int_create(   0, 8, 1, 5)));
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 9, 1, TOKEN_SEMICOLON)));
        list_push(expected_tokens, test_token_create_ptr((Token){.kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 10}, .length = 0}}));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}
static void test_lexer_if_num_int_eq_eq_1234_printf_hello_sc(void) {
    const TextView source = text_view_from_literal("if (num_int == 1234) printf(\"hello\");");
    Context* const lexer_context = context_create(source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    { // builds the list
        // if
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 0, 2, TOKEN_KW_IF)));
        // (
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 3, 1, TOKEN_PARENTHESES_LEFT)));
        // num_int
        list_push(expected_tokens, test_token_create_ptr(test_token_ident_create(       0, 4, 7, expected_context, text_view_from_literal("num_int"))));
        // ==
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 12, 2, TOKEN_EQUALS_EQUALS)));
        // 1234
        list_push(expected_tokens, test_token_create_ptr(test_token_const_int_create(   0, 15, 4, 1234)));
        // )
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 19, 1, TOKEN_PARENTHESES_RIGHT)));
        // printf
        list_push(expected_tokens, test_token_create_ptr(test_token_ident_create(       0, 21, 6, expected_context, text_view_from_literal("printf"))));
        // (
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 27, 1, TOKEN_PARENTHESES_LEFT)));
        // "hello"
        list_push(expected_tokens, test_token_create_ptr(test_token_string_literal_create(0, 28, 7, expected_context, text_view_from_literal("hello"))));
        // )
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 35, 1, TOKEN_PARENTHESES_RIGHT)));
        // ;
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 36, 1, TOKEN_SEMICOLON)));
        // EOF
        list_push(expected_tokens, test_token_create_ptr((Token){.kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 37}, .length = 0}}));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

// ---------------- Step 4c regression tests ---------------- //
// One test per input from the regression list in roadmaps/testing-roadmap.md, Step 4c.
// The error cases (empty and multi-character constants, 123abc) only check the tokens and that lexing recovers.
// Checking their diagnostics is Step 8.

/**
 * Regression: leading whitespace before the first token (" x").
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_space_x(void) {
    const TextView source = text_view_from_literal(" x");
    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens,
                  test_token_create_ptr(
                      test_token_ident_create(0, 1, 1, expected_context, text_view_from_literal("x"))));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 2}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

/**
 * Regression: trailing whitespace after the last token ("x\n"). EOF sits after the newline.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_x_newline(void) {
    const TextView source = text_view_from_literal("x\n");
    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens,
                  test_token_create_ptr(
                      test_token_ident_create(0, 0, 1, expected_context, text_view_from_literal("x"))));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 2}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

/**
 * Regression: a file that is just "+", i.e. a punctuator that could grow (++ isn't in C~~, but the lexer peeks
 * past it) ending exactly at EOF.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_plus(void) {
    const TextView source = text_view_from_literal("+");
    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 0, 1, TOKEN_PLUS)));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 1}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

/**
 * Regression: "==;" must split as == then ;, since "==;" isn't a punctuator (maximal munch, ADR-014).
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_eq_eq_sc(void) {
    const TextView source = text_view_from_literal("==;");
    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 0, 2, TOKEN_EQUALS_EQUALS)));
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 2, 1, TOKEN_SEMICOLON)));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 3}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

/**
 * Regression: an empty character constant ('') is one CONST_INT token with value 0, and lexing continues after it.
 * The diagnostic (empty character constant) is checked in Step 8.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_empty_char(void) {
    const TextView source = text_view_from_literal("'' x");
    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens, test_token_create_ptr(test_token_const_int_create(0, 0, 2, 0)));
        list_push(expected_tokens,
                  test_token_create_ptr(
                      test_token_ident_create(0, 3, 1, expected_context, text_view_from_literal("x"))));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 4}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

/**
 * Regression: a multi-character constant ('ab') is one CONST_INT token, valued as its first character, and lexing
 * continues after it. The diagnostic (multi-character constant) is checked in Step 8.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_multi_char(void) {
    const TextView source = text_view_from_literal("'ab' x");
    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens, test_token_create_ptr(test_token_const_int_create(0, 0, 4, 'a')));
        list_push(expected_tokens,
                  test_token_create_ptr(
                      test_token_ident_create(0, 5, 1, expected_context, text_view_from_literal("x"))));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 6}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

/**
 * Regression: "123abc" is ONE invalid CONST_INT token (value 0), not 123 followed by the identifier abc, and lexing
 * continues after it. The diagnostic (invalid numerical sequence) is checked in Step 8.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_123abc(void) {
    const TextView source = text_view_from_literal("123abc;");
    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens, test_token_create_ptr(test_token_const_int_create(0, 0, 6, 0)));
        list_push(expected_tokens, test_token_create_ptr(test_token_punc_keyword_create(0, 6, 1, TOKEN_SEMICOLON)));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 7}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
}

/**
 * Regression: a 4095-byte string literal (the old fixed decode buffer's limit, now gone) is one token holding all
 * 4095 bytes, not truncated.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_string_4095(void) {
    enum { CONTENT_LENGTH = 4095, SOURCE_LENGTH = CONTENT_LENGTH + 2 }; // + 2 for the quotes
    char* const buffer = utils_xmalloc(SOURCE_LENGTH);
    buffer[0] = '\"';
    memset(buffer + 1, 'a', CONTENT_LENGTH);
    buffer[SOURCE_LENGTH - 1] = '\"';

    Context* const lexer_context = context_create(
        source_text_create(text_create_from_buffer(buffer, SOURCE_LENGTH), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(
        source_text_create(text_create_from_buffer(buffer, SOURCE_LENGTH), "<expected>"));
    List* expected_tokens = list_create(free);
    {
        // builds the list
        list_push(expected_tokens,
                  test_token_create_ptr(test_token_string_literal_create(
                      0, 0, SOURCE_LENGTH, expected_context, text_view_create(CONTENT_LENGTH, buffer + 1))));
        list_push(expected_tokens, test_token_create_ptr((Token)
        {
            .kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = SOURCE_LENGTH}, .length = 0}
        }));
    }

    CHECK(are_token_lists_equal(lexer_tokens, expected_tokens, lexer_context, expected_context));

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected_tokens);
    context_destroy(expected_context);
    free(buffer);
}

/**
 * Regression: a 4096-byte string literal (one byte past the old fixed decode buffer's limit, now gone) is still
 * exactly one token holding all 4096 bytes, and lexing continues after it. There is no length limit
 * (implementation-defined.md), so this is not an error case.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_string_4096(void) {
    enum { CONTENT_LENGTH = 4096, SOURCE_LENGTH = CONTENT_LENGTH + 3 }; // + 2 for the quotes, + 1 for a trailing ';'
    char* const buffer = utils_xmalloc(SOURCE_LENGTH);
    buffer[0] = '\"';
    memset(buffer + 1, 'a', CONTENT_LENGTH);
    buffer[SOURCE_LENGTH - 2] = '\"';
    buffer[SOURCE_LENGTH - 1] = ';';

    Context* const context = context_create(
        source_text_create(text_create_from_buffer(buffer, SOURCE_LENGTH), "<source>"));
    Lexer* lexer = lexer_create(context, 0);
    List* tokens = lexer_lex_source_text(lexer);

    if (CHECK(list_get_size(tokens) == 3)) {
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        const Token* const third = list_at(tokens, 2);
        CHECK(first->kind == TOKEN_STRING_LITERAL);
        CHECK(first->range.position.offset == 0 && first->range.length == SOURCE_LENGTH - 1);
        const Text* const literal = list_at_const(context->string_literals_table, first->as.id);
        CHECK(literal->length == CONTENT_LENGTH);
        CHECK(memcmp(literal->data, buffer + 1, CONTENT_LENGTH) == 0);
        CHECK(second->kind == TOKEN_SEMICOLON);
        CHECK(second->range.position.offset == SOURCE_LENGTH - 1);
        CHECK(token_is_eof(*third));
    }

    list_destroy(tokens);
    lexer_destroy(lexer);
    context_destroy(context);
    free(buffer);
}

/**
 * Regression: identifier interning. In "a b a", both a-s get the same id, b gets a different one, and the table
 * holds exactly two identifiers.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_identifier_interning_a_b_a(void) {
    const TextView source = text_view_from_literal("a b a");
    Context* const context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(context, 0);
    List* tokens = lexer_lex_source_text(lexer);

    if (CHECK(list_get_size(tokens) == 4)) {
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        const Token* const third = list_at(tokens, 2);
        CHECK(first->kind == TOKEN_IDENT && second->kind == TOKEN_IDENT && third->kind == TOKEN_IDENT);
        CHECK(first->as.id == third->as.id);
        CHECK(first->as.id != second->as.id);
        CHECK(list_get_size(context->identifiers_table) == 2);
    }

    list_destroy(tokens);
    lexer_destroy(lexer);
    context_destroy(context);
}

/**
 * Regression (ADR-010): string literals are interned as bytes + length, so "a\0b" and "a" are different literals,
 * while two "a"-s share one id.
 * @note Written by Claude on 10-Oct-26, not by Tomer.
 */
static void test_lexer_string_interning_with_nul(void) {
    const TextView source = text_view_from_literal("\"a\\0b\" \"a\" \"a\"");
    Context* const context = context_create(
        source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(context, 0);
    List* tokens = lexer_lex_source_text(lexer);

    if (CHECK(list_get_size(tokens) == 4)) {
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        const Token* const third = list_at(tokens, 2);
        CHECK(first->kind == TOKEN_STRING_LITERAL && second->kind == TOKEN_STRING_LITERAL && third->kind ==
              TOKEN_STRING_LITERAL);
        CHECK(first->as.id != second->as.id);
        CHECK(second->as.id == third->as.id);
        CHECK(list_get_size(context->string_literals_table) == 2);
    }

    list_destroy(tokens);
    lexer_destroy(lexer);
    context_destroy(context);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);
    RUN_TEST(test_lexer_empty_buffer);
    RUN_TEST(test_lexer_single_punctuator);
    RUN_TEST(test_lexer_single_keyword);
    RUN_TEST(test_lexer_single_numeric);
    RUN_TEST(test_lexer_single_char);
    RUN_TEST(test_lexer_single_string);
    RUN_TEST(test_lexer_single_identifier);
    RUN_TEST(test_lexer_int_x_eq_5_sc);
    RUN_TEST(test_lexer_if_num_int_eq_eq_1234_printf_hello_sc);
    // Step 4c regression tests (written by Claude)
    RUN_TEST(test_lexer_space_x);
    RUN_TEST(test_lexer_x_newline);
    RUN_TEST(test_lexer_plus);
    RUN_TEST(test_lexer_eq_eq_sc);
    RUN_TEST(test_lexer_empty_char);
    RUN_TEST(test_lexer_multi_char);
    RUN_TEST(test_lexer_123abc);
    RUN_TEST(test_lexer_string_4095);
    RUN_TEST(test_lexer_string_4096);
    RUN_TEST(test_lexer_identifier_interning_a_b_a);
    RUN_TEST(test_lexer_string_interning_with_nul);
    return harness_finish();
}
