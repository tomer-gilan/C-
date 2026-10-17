//
// Written by Claude on 16-Oct-26.
//
// Parser tests for slice 1: expressions, statements, function definitions, the translation unit, whole programs
// through parser_parse, and inputs that must be rejected. Each valid-input test lexes a source string, calls one parse
// function and checks the nodes it returns, the parser's error flag (parser_has_error), and the next token (what the
// parse function left unconsumed). Each error test checks that parser_parse returns NULL, sets the error flag and
// reports exactly one diagnostic, with the expected code and offset.
// Expected values come from SUBSET.md, not from the parser's output.
//

#include <string.h>

#include "Harness.h"
#include "Lexer.h"
#include "Parser.h"
#include "SourceText.h"
#include "Statement.h"
#include "Text.h"

/** Everything one parse needs. The AST lives in @c context 's arena, so it's valid until the fixture is destroyed. */
typedef struct {
    Context* context;
    List* tokens;
    Parser* parser;
} ParseFixture;

/** Lexes @b source and creates a @c Parser on its tokens. Ownership of @b source remains with the caller. */
static ParseFixture parse_fixture_create(const char* const source) {
    Context* const context = context_create(
        source_text_create(text_create_from_buffer(source, (int)strlen(source)), "<source>"),
        compilation_flags_basic());
    Lexer* const lexer = lexer_create(context, 0);
    List* const tokens = lexer_lex_source_text(lexer);
    lexer_destroy(lexer);

    return (ParseFixture){
        .context = context,
        .tokens = tokens,
        .parser = parser_create(tokens, context)
    };
}
static void parse_fixture_destroy(const ParseFixture fixture) {
    parser_destroy(fixture.parser);
    list_destroy(fixture.tokens);
    context_destroy(fixture.context);
}

static bool next_token_is(const ParseFixture* const fixture, const TokenKind kind) {
    return parser_peek(fixture->parser)->kind == kind;
}
static bool range_is(const Range range, const int offset, const int length) {
    return range.position.offset == offset && range.length == length;
}
static int block_item_count(const StatementNode* const compound) {
    int count = 0;
    for (const BlockItem* item = compound->as.compound_statement.head; item != NULL; item = item->next)
        count++;
    return count;
}
/** @return the statement of the @b index 'th block item, or @c NULL if there's no such item or it isn't a statement. */
static const StatementNode* block_item_statement_at(const StatementNode* const compound, const int index) {
    const BlockItem* item = compound->as.compound_statement.head;
    for (int i = 0; i < index && item != NULL; i++)
        item = item->next;
    if (item == NULL || item->kind != BLOCK_ITEM_KIND_STATEMENT)
        return NULL;
    return item->as.statement;
}
/** Checks that @b statement is @c return @b value; (an integer constant). */
static void check_return_of_int_constant(const StatementNode* const statement, const int value) {
    if (!CHECK(statement != NULL))
        return;
    if (!CHECK(statement->kind == STATEMENT_KIND_RETURN))
        return;
    const ExpressionNode* const expression = statement->as.return_statement.expression;
    if (!CHECK(expression != NULL))
        return;
    CHECK(expression->kind == EXPRESSION_KIND_INT_CONSTANT);
    CHECK(expression->as.expression_int_constant.value == value);
}

// ---------------- Expressions ---------------- //

/** "42;": an integer constant. The ';' isn't part of the expression, so it's left as the next token. */
static void test_parser_expression_42_sc(void) {
    const ParseFixture fixture = parse_fixture_create("42;");

    const ExpressionNode* const expression = expression_node_parse(fixture.parser);
    if (CHECK(expression != NULL)) {
        CHECK(expression->kind == EXPRESSION_KIND_INT_CONSTANT);
        CHECK(expression->as.expression_int_constant.value == 42);
        CHECK(range_is(expression->range, 0, 2));
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_SEMICOLON));

    parse_fixture_destroy(fixture);
}
/** "0": the smallest constant, and the whole input. */
static void test_parser_expression_0(void) {
    const ParseFixture fixture = parse_fixture_create("0");

    const ExpressionNode* const expression = expression_node_parse(fixture.parser);
    if (CHECK(expression != NULL)) {
        CHECK(expression->kind == EXPRESSION_KIND_INT_CONSTANT);
        CHECK(expression->as.expression_int_constant.value == 0);
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}

// ---------------- Statements ---------------- //

/** "return 42;" */
static void test_parser_statement_return_42_sc(void) {
    const ParseFixture fixture = parse_fixture_create("return 42;");

    check_return_of_int_constant(statement_node_parse(fixture.parser), 42);
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "{}": an empty block. Its range spans both braces. */
static void test_parser_statement_empty_block(void) {
    const ParseFixture fixture = parse_fixture_create("{}");

    const StatementNode* const block = statement_node_parse(fixture.parser);
    if (CHECK(block != NULL) && CHECK(block->kind == STATEMENT_KIND_COMPOUND)) {
        CHECK(block_item_count(block) == 0);
        CHECK(range_is(block->range, 0, 2));
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "{{}}": a block whose only item is an empty block. */
static void test_parser_statement_nested_empty_block(void) {
    const ParseFixture fixture = parse_fixture_create("{{}}");

    const StatementNode* const outer = statement_node_parse(fixture.parser);
    if (CHECK(outer != NULL) && CHECK(outer->kind == STATEMENT_KIND_COMPOUND)) {
        CHECK(range_is(outer->range, 0, 4));
        CHECK(block_item_count(outer) == 1);

        const StatementNode* const inner = block_item_statement_at(outer, 0);
        if (CHECK(inner != NULL) && CHECK(inner->kind == STATEMENT_KIND_COMPOUND)) {
            CHECK(block_item_count(inner) == 0);
            CHECK(range_is(inner->range, 1, 2));
        }
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "{}{}": one call parses one statement, so the second block is left as the next tokens, and a second call parses it. */
static void test_parser_statement_two_blocks(void) {
    const ParseFixture fixture = parse_fixture_create("{}{}");

    const StatementNode* const first = statement_node_parse(fixture.parser);
    if (CHECK(first != NULL) && CHECK(first->kind == STATEMENT_KIND_COMPOUND)) {
        CHECK(block_item_count(first) == 0);
        CHECK(range_is(first->range, 0, 2));
    }
    CHECK(next_token_is(&fixture, TOKEN_BRACE_LEFT));

    const StatementNode* const second = statement_node_parse(fixture.parser);
    if (CHECK(second != NULL) && CHECK(second->kind == STATEMENT_KIND_COMPOUND)) {
        CHECK(block_item_count(second) == 0);
        CHECK(range_is(second->range, 2, 2));
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "{return 42;}": a block with one statement. */
static void test_parser_statement_block_with_return(void) {
    const ParseFixture fixture = parse_fixture_create("{return 42;}");

    const StatementNode* const block = statement_node_parse(fixture.parser);
    if (CHECK(block != NULL) && CHECK(block->kind == STATEMENT_KIND_COMPOUND)) {
        CHECK(block_item_count(block) == 1);
        check_return_of_int_constant(block_item_statement_at(block, 0), 42);
        CHECK(range_is(block->range, 0, 12));
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "{{return 1;} return 2;}": block items keep their source order. */
static void test_parser_statement_block_items_in_order(void) {
    const ParseFixture fixture = parse_fixture_create("{{return 1;} return 2;}");

    const StatementNode* const block = statement_node_parse(fixture.parser);
    if (CHECK(block != NULL) && CHECK(block->kind == STATEMENT_KIND_COMPOUND)) {
        CHECK(block_item_count(block) == 2);

        const StatementNode* const inner = block_item_statement_at(block, 0);
        if (CHECK(inner != NULL) && CHECK(inner->kind == STATEMENT_KIND_COMPOUND)) {
            CHECK(block_item_count(inner) == 1);
            check_return_of_int_constant(block_item_statement_at(inner, 0), 1);
        }
        check_return_of_int_constant(block_item_statement_at(block, 1), 2);
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}

// ---------------- External definitions and the translation unit ---------------- //
// external_definition_node_parse is static, so function definitions are tested through translation_unit_node_parse.

static int external_definition_count(const TranslationUnitNode* const translation_unit) {
    int count = 0;
    for (const ExternalDefinitionNode* definition = translation_unit->head; definition != NULL; definition = definition->next)
        count++;
    return count;
}
/** @return the @b index 'th external definition, or @c NULL if there's no such definition. */
static const ExternalDefinitionNode* external_definition_at(const TranslationUnitNode* const translation_unit, const int index) {
    const ExternalDefinitionNode* definition = translation_unit->head;
    for (int i = 0; i < index && definition != NULL; i++)
        definition = definition->next;
    return definition;
}
/** @return true iff the interned identifier @b id is spelled @b name . */
static bool identifier_is(const ParseFixture* const fixture, const int id, const char* const name) {
    const TextView* const spelling = list_at_const(fixture->context->identifiers_table, id);
    return text_view_equals(*spelling, text_view_create((int)strlen(name), name));
}
/** @return the body of @b definition if it's a function definition named @b name with a block body, @c NULL otherwise. */
static const StatementNode* check_function_definition(
    const ParseFixture* const fixture,
    const ExternalDefinitionNode* const definition,
    const char* const name) {
    if (!CHECK(definition != NULL) || !CHECK(definition->kind == EXTERNAL_DEFINITION_KIND_FUNCTION))
        return NULL;
    const ExternalDefinitionFunction* const function = &definition->as.external_function_definition;
    CHECK(identifier_is(fixture, function->declarator.identifier.as.id, name));
    if (!CHECK(function->body != NULL) || !CHECK(function->body->kind == STATEMENT_KIND_COMPOUND))
        return NULL;
    return function->body;
}

/** "int main(void) { return 2; }": the slice 1 program. The definition spans all of it. */
static void test_parser_function_definition_main(void) {
    const ParseFixture fixture = parse_fixture_create("int main(void) { return 2; }");

    const TranslationUnitNode* const translation_unit = translation_unit_node_parse(fixture.parser);
    if (CHECK(translation_unit != NULL)) {
        CHECK(external_definition_count(translation_unit) == 1);
        const ExternalDefinitionNode* const definition = external_definition_at(translation_unit, 0);
        const StatementNode* const body = check_function_definition(&fixture, definition, "main");
        if (body != NULL) {
            CHECK(block_item_count(body) == 1);
            check_return_of_int_constant(block_item_statement_at(body, 0), 2);
            CHECK(range_is(body->range, 15, 13));
            CHECK(range_is(definition->range, 0, 28));
        }
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "int foo(void){return 0;}": the name is any identifier, not just main. */
static void test_parser_function_definition_any_name(void) {
    const ParseFixture fixture = parse_fixture_create("int foo(void){return 0;}");

    const TranslationUnitNode* const translation_unit = translation_unit_node_parse(fixture.parser);
    if (CHECK(translation_unit != NULL)) {
        CHECK(external_definition_count(translation_unit) == 1);
        const StatementNode* const body =
            check_function_definition(&fixture, external_definition_at(translation_unit, 0), "foo");
        if (body != NULL) {
            CHECK(block_item_count(body) == 1);
            check_return_of_int_constant(block_item_statement_at(body, 0), 0);
        }
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "int main(void){}": an empty body is still a block. */
static void test_parser_function_definition_empty_body(void) {
    const ParseFixture fixture = parse_fixture_create("int main(void){}");

    const TranslationUnitNode* const translation_unit = translation_unit_node_parse(fixture.parser);
    if (CHECK(translation_unit != NULL)) {
        CHECK(external_definition_count(translation_unit) == 1);
        const StatementNode* const body =
            check_function_definition(&fixture, external_definition_at(translation_unit, 0), "main");
        if (body != NULL)
            CHECK(block_item_count(body) == 0);
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/**
 * "int f(void){return 1;} int g(void){return 2;}": definitions keep their source order, and the translation unit spans
 * from the first one's start to the last one's end.
 */
static void test_parser_translation_unit_two_functions(void) {
    const ParseFixture fixture = parse_fixture_create("int f(void){return 1;} int g(void){return 2;}");

    const TranslationUnitNode* const translation_unit = translation_unit_node_parse(fixture.parser);
    if (CHECK(translation_unit != NULL)) {
        CHECK(external_definition_count(translation_unit) == 2);
        CHECK(range_is(translation_unit->range, 0, 45));

        const ExternalDefinitionNode* const first = external_definition_at(translation_unit, 0);
        const StatementNode* const first_body = check_function_definition(&fixture, first, "f");
        if (first_body != NULL) {
            check_return_of_int_constant(block_item_statement_at(first_body, 0), 1);
            CHECK(range_is(first->range, 0, 22));
        }
        const ExternalDefinitionNode* const second = external_definition_at(translation_unit, 1);
        const StatementNode* const second_body = check_function_definition(&fixture, second, "g");
        if (second_body != NULL) {
            check_return_of_int_constant(block_item_statement_at(second_body, 0), 2);
            CHECK(range_is(second->range, 23, 22));
        }
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** "int f(void){return 1;} int f(void){return 2;}": two definitions with the same name parse; rejecting them is sema's job. */
static void test_parser_translation_unit_same_name_twice(void) {
    const ParseFixture fixture = parse_fixture_create("int f(void){return 1;} int f(void){return 2;}");

    const TranslationUnitNode* const translation_unit = translation_unit_node_parse(fixture.parser);
    if (CHECK(translation_unit != NULL)) {
        CHECK(external_definition_count(translation_unit) == 2);
        check_function_definition(&fixture, external_definition_at(translation_unit, 0), "f");
        check_function_definition(&fixture, external_definition_at(translation_unit, 1), "f");
    }
    CHECK(!parser_has_error(fixture.parser));

    parse_fixture_destroy(fixture);
}

// ---------------- The whole parser (parser_parse) ---------------- //

/** The slice 1 program as it would sit in a file: newlines, indentation and a trailing newline. */
static void test_parser_parse_slice_1_program(void) {
    const ParseFixture fixture = parse_fixture_create(
        "int main(void) {\n"
        "    return 2;\n"
        "}\n");

    const TranslationUnitNode* const translation_unit = parser_parse(fixture.parser);
    if (CHECK(translation_unit != NULL)) {
        CHECK(external_definition_count(translation_unit) == 1);
        const StatementNode* const body =
            check_function_definition(&fixture, external_definition_at(translation_unit, 0), "main");
        if (body != NULL) {
            CHECK(block_item_count(body) == 1);
            check_return_of_int_constant(block_item_statement_at(body, 0), 2);
        }
        CHECK(range_is(translation_unit->range, 0, 32)); // up to and including the final }, not the trailing newline
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}
/** Everything slice 1 has, together: several functions, nested and empty blocks, and statements after a block. */
static void test_parser_parse_whole_program(void) {
    const ParseFixture fixture = parse_fixture_create(
        "int helper(void) {\n"
        "    {}\n"
        "    return 7;\n"
        "}\n"
        "\n"
        "int main(void) {\n"
        "    {\n"
        "        { return 1; }\n"
        "    }\n"
        "    return 0;\n"
        "}\n");

    const TranslationUnitNode* const translation_unit = parser_parse(fixture.parser);
    if (CHECK(translation_unit != NULL)) {
        CHECK(external_definition_count(translation_unit) == 2);

        const StatementNode* const helper_body =
            check_function_definition(&fixture, external_definition_at(translation_unit, 0), "helper");
        if (helper_body != NULL) {
            CHECK(block_item_count(helper_body) == 2);
            const StatementNode* const empty_block = block_item_statement_at(helper_body, 0);
            if (CHECK(empty_block != NULL) && CHECK(empty_block->kind == STATEMENT_KIND_COMPOUND))
                CHECK(block_item_count(empty_block) == 0);
            check_return_of_int_constant(block_item_statement_at(helper_body, 1), 7);
        }

        const StatementNode* const main_body =
            check_function_definition(&fixture, external_definition_at(translation_unit, 1), "main");
        if (main_body != NULL) {
            CHECK(block_item_count(main_body) == 2);
            const StatementNode* const outer = block_item_statement_at(main_body, 0);
            if (CHECK(outer != NULL) && CHECK(outer->kind == STATEMENT_KIND_COMPOUND)) {
                CHECK(block_item_count(outer) == 1);
                const StatementNode* const inner = block_item_statement_at(outer, 0);
                if (CHECK(inner != NULL) && CHECK(inner->kind == STATEMENT_KIND_COMPOUND)) {
                    CHECK(block_item_count(inner) == 1);
                    check_return_of_int_constant(block_item_statement_at(inner, 0), 1);
                }
            }
            check_return_of_int_constant(block_item_statement_at(main_body, 1), 0);
        }
    }
    CHECK(!parser_has_error(fixture.parser));
    CHECK(next_token_is(&fixture, TOKEN_EOF));

    parse_fixture_destroy(fixture);
}

// ---------------- Errors (parser_parse) ---------------- //
// The parser stops at the first syntax error (ADR-020): parser_parse returns NULL, the error flag is set, and exactly
// one diagnostic is reported. The expected code and offset of each one were worked out by hand from the input (the
// offset of the token the parser found instead; EOF is at the end of the input), not copied from the parser's output.

/**
 * Checks that parsing @b source as a whole program fails: @c NULL result, the error flag set, and exactly one
 * diagnostic, with code @b code at byte offset @b offset .
 */
static void check_parse_fails(const char* const source, const DiagnosticCode code, const int offset) {
    const ParseFixture fixture = parse_fixture_create(source);

    CHECK(parser_parse(fixture.parser) == NULL);
    CHECK(parser_has_error(fixture.parser));

    const List* const diagnostics = fixture.context->diagnostic_report_table;
    if (CHECK(list_get_size(diagnostics) == 1)) {
        const Diagnostic* const diagnostic = list_at_const(diagnostics, 0);
        CHECK(diagnostic->code == code);
        CHECK(diagnostic->range.position.offset == offset);
    }

    parse_fixture_destroy(fixture);
}

/** "": SUBSET.md A.2.4: a translation unit has at least one external definition. Reported at EOF. */
static void test_parser_error_empty_file(void) {
    check_parse_fails("", DIAGNOSTIC_CODE_PARSER_MISSING_EXTERNAL_DEFINITION, 0);
}
/** "   ": whitespace only is an empty translation unit too. EOF is after the 3 spaces. */
static void test_parser_error_whitespace_only(void) {
    check_parse_fails("   ", DIAGNOSTIC_CODE_PARSER_MISSING_EXTERNAL_DEFINITION, 3);
}
/** "int main(void) { return 42 }": the ';' after the return expression is missing. Reported at the '}'. */
static void test_parser_error_missing_semicolon(void) {
    check_parse_fails("int main(void) { return 42 }", DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, 27);
}
/** "int main(void) { return 2;": the closing '}' is missing, so the block asks for another statement at EOF. */
static void test_parser_error_missing_right_brace(void) {
    check_parse_fails("int main(void) { return 2;", DIAGNOSTIC_CODE_PARSER_INVALID_STATEMENT, 26);
}
/** "int (void) { return 2; }": the function has no name. Reported at the '('. */
static void test_parser_error_missing_function_name(void) {
    check_parse_fails("int (void) { return 2; }", DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, 4);
}
/** "int main(void) return 2;": the body isn't a block. Reported at 'return', where the '{' should be. */
static void test_parser_error_missing_left_brace(void) {
    check_parse_fails("int main(void) return 2;", DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, 15);
}
/** "int main(void": EOF in the middle of the parameter list, where the ')' should be. */
static void test_parser_error_eof_in_parameter_list(void) {
    check_parse_fails("int main(void", DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, 13);
}
/**
 * "int main(int) { return 2; }": parameters come in slice 9, and an unnamed one stays an error in a definition.
 * Reported at 'int', where the 'void' should be; the ')' that's then also missing must not be reported too.
 */
static void test_parser_error_unnamed_parameter(void) {
    check_parse_fails("int main(int) { return 2; }", DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, 9);
}
/** "int main(void) { return 2; } }": a stray '}' after the last definition, where an 'int' should be. */
static void test_parser_error_stray_right_brace(void) {
    check_parse_fails("int main(void) { return 2; } }", DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, 29);
}
/** "int main(void) { return 2; } 5": a token that can't start an external definition, where an 'int' should be. */
static void test_parser_error_trailing_token(void) {
    check_parse_fails("int main(void) { return 2; } 5", DIAGNOSTIC_CODE_PARSER_EXPECTED_TOKEN, 29);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);

    RUN_TEST(test_parser_expression_42_sc);
    RUN_TEST(test_parser_expression_0);

    RUN_TEST(test_parser_statement_return_42_sc);
    RUN_TEST(test_parser_statement_empty_block);
    RUN_TEST(test_parser_statement_nested_empty_block);
    RUN_TEST(test_parser_statement_two_blocks);
    RUN_TEST(test_parser_statement_block_with_return);
    RUN_TEST(test_parser_statement_block_items_in_order);

    RUN_TEST(test_parser_function_definition_main);
    RUN_TEST(test_parser_function_definition_any_name);
    RUN_TEST(test_parser_function_definition_empty_body);
    RUN_TEST(test_parser_translation_unit_two_functions);
    RUN_TEST(test_parser_translation_unit_same_name_twice);

    RUN_TEST(test_parser_parse_slice_1_program);
    RUN_TEST(test_parser_parse_whole_program);

    RUN_TEST(test_parser_error_empty_file);
    RUN_TEST(test_parser_error_whitespace_only);
    RUN_TEST(test_parser_error_missing_semicolon);
    RUN_TEST(test_parser_error_missing_right_brace);
    RUN_TEST(test_parser_error_missing_function_name);
    RUN_TEST(test_parser_error_missing_left_brace);
    RUN_TEST(test_parser_error_eof_in_parameter_list);
    RUN_TEST(test_parser_error_unnamed_parameter);
    RUN_TEST(test_parser_error_stray_right_brace);
    RUN_TEST(test_parser_error_trailing_token);

    return harness_finish();
}
