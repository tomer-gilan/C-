//
// Written by tomer on 02-Oct-26.
//

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Harness.h"
#include "Lexer.h"
#include "Token.h"

/**
 * returns the @c Token @c List which was lexed from @b buffer.
 * @attention ownership of @b buffer remains with the callee.
 */
static List* test_lexer_lex_buffer(const char* const buffer, int const length) {
    Context* context = context_create(source_text_create_from_buffer(buffer, length));
    Lexer* lexer = lexer_create(context, 0);
    List* tokens = lexer_lex_source_text(lexer);
    lexer_destroy(lexer);
    context_destroy(context);
    return tokens;
}

static void test_lexer_empty_buffer(void) {
    List* tokens = test_lexer_lex_buffer("", 0);

    Token* first = list_at(tokens, 0);

    CHECK(list_get_size(tokens) == 1);
    CHECK(token_is_EOF(*(Token*)list_at(tokens, 0)));

    list_destroy(tokens);
}
static void test_lexer_single_punctuator(void) {
    for (TokenKind kind = TOKEN__PUNC_FIRST; kind <= TOKEN__PUNC_LAST; ++kind) {
        const StringView view = token_kind_spelling_table[kind];
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
        const StringView view = token_kind_spelling_table[kind];
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
        StringView spelling;
        int number;
    } test_numeric;
    test_numeric arr[] = {
        {.spelling = string_view_from_literal("0"), .number = 0},
        {.spelling = string_view_from_literal("1"), .number = 1},
        {.spelling = string_view_from_literal("2"), .number = 2},
        {.spelling = string_view_from_literal("3"), .number = 3},
        {.spelling = string_view_from_literal("4"), .number = 4},
        {.spelling = string_view_from_literal("5"), .number = 5},
        {.spelling = string_view_from_literal("6"), .number = 6},
        {.spelling = string_view_from_literal("7"), .number = 7},
        {.spelling = string_view_from_literal("8"), .number = 8},
        {.spelling = string_view_from_literal("9"), .number = 9},
        {.spelling = string_view_from_literal("10"), .number = 10},
        {.spelling = string_view_from_literal("2147483647"), .number = 2147483647},
        {.spelling = string_view_from_literal("4953728"), .number = 4953728},
        {.spelling = string_view_from_literal("111111111"), .number = 111111111},
        {.spelling = string_view_from_literal("245800002"), .number = 245800002}
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
        StringView spelling;
        int number;
    } test_char;
    test_char arr[] = {
        {.spelling = string_view_from_literal("'0'"), .number = '0'},
        {.spelling = string_view_from_literal("'1'"), .number = '1'},
        {.spelling = string_view_from_literal("'2'"), .number = '2'},
        {.spelling = string_view_from_literal("'3'"), .number = '3'},
        {.spelling = string_view_from_literal("'4'"), .number = '4'},
        {.spelling = string_view_from_literal("'5'"), .number = '5'},
        {.spelling = string_view_from_literal("'6'"), .number = '6'},
        {.spelling = string_view_from_literal("'7'"), .number = '7'},
        {.spelling = string_view_from_literal("'8'"), .number = '8'},
        {.spelling = string_view_from_literal("'9'"), .number = '9'},
        {.spelling = string_view_from_literal("'\\0'"), .number = '\0'},
        {.spelling = string_view_from_literal("'\\n'"), .number = '\n'},
        {.spelling = string_view_from_literal("'\\r'"), .number = '\r'},
        {.spelling = string_view_from_literal("'\\f'"), .number = '\f'},
        {.spelling = string_view_from_literal("'\\a'"), .number = '\a'},
        {.spelling = string_view_from_literal("'\\t'"), .number = '\t'},
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
        StringView spelling;
        char* literal;
    } test_string;
    test_string arr[] = {
        {.spelling = string_view_from_literal("\"\""),      .literal = ""},
        {.spelling = string_view_from_literal("\"'\""),     .literal = "'"},
        {.spelling = string_view_from_literal("\"''\""),    .literal = "''"},
        {.spelling = string_view_from_literal("\"'''\""),   .literal = "'''"},
        {.spelling = string_view_from_literal("\"        \""), .literal = "        "},
        //{.spelling = string_view_from_literal("\"\\0\""), .literal = "\0"},  ... unsupported right now
        {.spelling = string_view_from_literal("\"a\""), .literal = "a"},
        {.spelling = string_view_from_literal("\"ab\""), .literal = "ab"},
        {.spelling = string_view_from_literal("\"abc\""), .literal = "abc"},
        {.spelling = string_view_from_literal("\"a51fdg2g78\""), .literal = "a51fdg2g78"},
        {.spelling = string_view_from_literal("\"]\""), .literal = "]"},
        {.spelling = string_view_from_literal("\"]%S&@BVBSH@*(SG@*(S*#^@\""), .literal = "]%S&@BVBSH@*(SG@*(S*#^@"},
        {.spelling = string_view_from_literal("\"]%S&@\\nBVBSH@\\n*(SG\\r\\f\\n@*(S*#^@\""), .literal = "]%S&@\nBVBSH@\n*(SG\r\f\n@*(S*#^@"},
    };
    const int N = sizeof(arr) / sizeof(test_string);

    for (int i = 0; i < N; ++i) {
        const test_string test = arr[i];

        Context* context = context_create(source_text_create_from_buffer(test.spelling.data, test.spelling.length));
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
        StringView spelling;
        StringView identifier;
    } test_identifier;
    test_identifier arr[] = {
        {.spelling = string_view_from_literal("_"), .identifier = string_view_from_literal("_")},
        {.spelling = string_view_from_literal("a"), .identifier = string_view_from_literal("a")},
        {.spelling = string_view_from_literal("abcdefg"), .identifier = string_view_from_literal("abcdefg")},
        {.spelling = string_view_from_literal("int_but_not_quite"), .identifier = string_view_from_literal("int_but_not_quite")},
        {.spelling = string_view_from_literal("is_char_or_is"), .identifier = string_view_from_literal("is_char_or_is")},
        {.spelling = string_view_from_literal("not_if_is_but_else"), .identifier = string_view_from_literal("not_if_is_but_else")},
        {.spelling = string_view_from_literal("a1_1234"), .identifier = string_view_from_literal("a1_1234")},
        {.spelling = string_view_from_literal("_122345678"), .identifier = string_view_from_literal("_122345678")},
    };
    const int N = sizeof(arr) / sizeof(test_identifier);

    for (int i = 0; i < N; ++i) {
        const test_identifier test = arr[i];

        Context* context = context_create(source_text_create_from_buffer(test.spelling.data, test.spelling.length));
        Lexer* lexer = lexer_create(context, 0);
        List* tokens = lexer_lex_source_text(lexer);

        CHECK(list_get_size(tokens) == 2);
        const Token* const first = list_at(tokens, 0);
        const Token* const second = list_at(tokens, 1);
        CHECK(first->kind == TOKEN_IDENT);
        CHECK(list_contains(tokens, &test.identifier, string_view_list_equals));
        CHECK(token_is_EOF(*second));

        list_destroy(tokens);
        lexer_destroy(lexer);
        context_destroy(context);

        break;
    }
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
