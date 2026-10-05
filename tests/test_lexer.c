//
// Written by tomer on 02-Oct-26.
//

#include <assert.h>
#include <limits.h>
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
 * @attention ownership of @b buffer remains with the callee.
 */
static List* test_lexer_lex_buffer(const char* const buffer, int const length) {
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

    if (!range_equals(first->range, second->range)) // note: range_equals assumes the underling Context-s are the same.
                                                               // They are not in this case. However, it is valid since range_equals
                                                               // only checks the position offset and length, and this is what we
                                                               // want to find out.
        return false;
    if (token_is_EOF(*first) && token_is_EOF(*second))
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
        return utils_string_list_compare(
            list_at_const(context_first->string_literals_table, first->as.id),
            list_at_const(context_second->string_literals_table, second->as.id)
            ) == 0;
    }
    if (token_kind_is_keyword(kind) || token_kind_is_punctuator(kind)) {
        return true;
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

static void test_lexer_empty_buffer(void) {
    List* tokens = test_lexer_lex_buffer("", 0);

    CHECK(list_get_size(tokens) == 1);
    CHECK(token_is_EOF(*(Token*)list_at(tokens, 0)));

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
        CHECK(token_is_EOF(*second));

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
        CHECK(token_is_EOF(*second));

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
        CHECK(token_is_EOF(*second));

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
        CHECK(token_is_EOF(*second));

        list_destroy(tokens);
    }
}
static void test_lexer_single_string(void) {
    typedef struct {
        TextView spelling;
        char* literal;
    } test_string;
    test_string arr[] = {
        {.spelling = text_view_from_literal("\"\""),      .literal = ""},
        {.spelling = text_view_from_literal("\"'\""),     .literal = "'"},
        {.spelling = text_view_from_literal("\"''\""),    .literal = "''"},
        {.spelling = text_view_from_literal("\"'''\""),   .literal = "'''"},
        {.spelling = text_view_from_literal("\"        \""), .literal = "        "},
        //{.spelling = text_view_from_literal("\"\\0\""), .literal = "\0"},  ... unsupported right now
        {.spelling = text_view_from_literal("\"a\""), .literal = "a"},
        {.spelling = text_view_from_literal("\"ab\""), .literal = "ab"},
        {.spelling = text_view_from_literal("\"abc\""), .literal = "abc"},
        {.spelling = text_view_from_literal("\"a51fdg2g78\""), .literal = "a51fdg2g78"},
        {.spelling = text_view_from_literal("\"]\""), .literal = "]"},
        {.spelling = text_view_from_literal("\"]%S&@BVBSH@*(SG@*(S*#^@\""), .literal = "]%S&@BVBSH@*(SG@*(S*#^@"},
        {.spelling = text_view_from_literal("\"]%S&@\\nBVBSH@\\n*(SG\\r\\f\\n@*(S*#^@\""), .literal = "]%S&@\nBVBSH@\n*(SG\r\f\n@*(S*#^@"},
    };
    const int N = sizeof(arr) / sizeof(test_string);

    for (int i = 0; i < N; ++i) {
        const test_string test = arr[i];

        Context* context = context_create(source_text_create(text_create_from_buffer(test.spelling.data, test.spelling.length), "<source>"));
        Lexer* lexer = lexer_create(context, 0);
        List* tokens = lexer_lex_source_text(lexer);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == TOKEN_STRING_LITERAL);
        CHECK(0 == strcmp(list_at(context->string_literals_table, first->as.id), test.literal)); // TODO: replace strcmp with memcmp once string literals are saved as StringView.
        CHECK(token_is_EOF(*second));

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
        CHECK(token_is_EOF(*second));

        list_destroy(tokens);
        lexer_destroy(lexer);
        context_destroy(context);
    }
}
static void test_lexer_int_x_eq_5_sc(void) {
    /*const TextView source = text_view_from_literal("int x = 5;");
    Context* lexer_context = context_create(source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    Lexer* lexer = lexer_create(lexer_context, 0);
    List* lexer_tokens = lexer_lex_source_text(lexer);

    Context* const expected_context = context_create(source_text_create(text_create_from_buffer(source.data, source.length), "<source>"));
    List* expected = list_create(free);
    {
        const Token arr[] = {
            {.kind = TOKEN_KW_INT, .range = {.position = {.source_text_index = 0, .offset = 0}, .length = 3}},
            {.kind = TOKEN_IDENT, .range = {.position = {.source_text_index = 0, .offset = 4}, .length = 1}},
            {.kind = TOKEN_EQUALS, .range = {.position = {.source_text_index = 0, .offset = 6}, .length = 1}},
            {.kind = TOKEN_CONST_INT, .range = {.position = {.source_text_index = 0, .offset = 8}, .length = 1}},
            {.kind = TOKEN_SEMICOLON, .range = {.position = {.source_text_index = 0, .offset = 9}, .length = 1}},
            {.kind = TOKEN_EOF, .range = {.position = {.source_text_index = 0, .offset = 10}, .length = 0}},
        };
        const int N = sizeof(arr) / sizeof(Token);
        for (int i = 0; i < N; ++i) {
            Token* t = malloc(sizeof(Token));
            *t = arr[i];
            list_push(expected, t);
        }
    }

    are_token_lists_equal(lexer_tokens, expected, lexer_context);

    list_destroy(lexer_tokens);
    lexer_destroy(lexer);
    context_destroy(lexer_context);

    list_destroy(expected);*/
}

int main(void) {
    RUN_TEST(test_lexer_empty_buffer);
    RUN_TEST(test_lexer_single_punctuator);
    RUN_TEST(test_lexer_single_keyword);
    RUN_TEST(test_lexer_single_numeric);
    RUN_TEST(test_lexer_single_char);
    RUN_TEST(test_lexer_single_string);
    RUN_TEST(test_lexer_single_identifier);
    return harness_finish();
}
