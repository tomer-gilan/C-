//
// Written by Claude on 19-Oct-26.
//
// Sema tests for slice 1. Each test lexes, parses and analyzes a source string. A valid program must give no
// diagnostics and a type for every expression. An invalid one must give exactly one diagnostic (sema stops at the first
// error, ADR-025), with the expected code and offset. Until the typed dump exists (parser Step 2), tests read sema's
// table directly, through each expression node's id (ADR-023).
// Expected values come from SUBSET.md and the C standard, not from sema's output.
//

#include <string.h>

#include "Context.h"
#include "Harness.h"
#include "Lexer.h"
#include "Parser.h"
#include "Sema.h"
#include "SourceText.h"
#include "Statement.h"
#include "Text.h"

/**
 * Everything one analysis needs. The AST, the symbols and sema's table live in @c context 's arena, so they're valid
 * until the fixture is destroyed. If the source didn't parse, @c parsed is false, @c ast is @c NULL and sema didn't run.
 */
typedef struct {
    Context* context;
    List* tokens;
    bool parsed;
    const TranslationUnitNode* ast;
    int expression_count;   ///< the parser's count, which sema's table must match
    SemaResult result;
} SemaFixture;

/** Lexes, parses and (if the parse reported nothing) analyzes @b source . Ownership of @b source remains with the caller. */
static SemaFixture sema_fixture_create(const char* const source) {
    Context* const context = context_create(
        source_text_create(text_create_from_buffer(source, (int)strlen(source)), "<source>"),
        compilation_flags_basic());
    Lexer* const lexer = lexer_create(context, 0);
    List* const tokens = lexer_lex_source_text(lexer);
    lexer_destroy(lexer);

    Parser* const parser = parser_create(tokens, context);
    const TranslationUnitNode* const ast = parser_parse(parser);
    const int expression_count = parser_get_expression_id_count(parser);
    parser_destroy(parser);

    SemaFixture fixture = {
        .context = context,
        .tokens = tokens,
        .parsed = ast != NULL && list_get_size(context->diagnostic_report_table) == 0,
        .ast = ast,
        .expression_count = expression_count,
        .result = { .expressions_semantics = NULL, .expressions_count = 0 }
    };
    if (fixture.parsed) {
        Sema* const sema = sema_create(context, ast, expression_count);
        fixture.result = sema_analyze(sema);
        sema_destroy(sema);
    }
    return fixture;
}
static void sema_fixture_destroy(const SemaFixture fixture) {
    list_destroy(fixture.tokens);
    context_destroy(fixture.context);
}

/** @return sema's entry for @b expression , found by its id. */
static const ExpressionSemantics* semantics_of(const SemaFixture* const fixture, const ExpressionNode* const expression) {
    return &fixture->result.expressions_semantics[expression->id];
}
/** @return the @b index 'th external definition's function, or @c NULL if there's no such definition. */
static const ExternalDefinitionFunction* function_at(const TranslationUnitNode* const translation_unit, const int index) {
    const ExternalDefinitionNode* definition = translation_unit->head;
    for (int i = 0; i < index && definition != NULL; i++)
        definition = definition->next;
    if (definition == NULL || definition->kind != EXTERNAL_DEFINITION_KIND_FUNCTION)
        return NULL;
    return &definition->as.external_function_definition;
}
/** @return the expression of @b function 's first statement, if it's a @c return , @c NULL otherwise (also for a @c NULL @b function ). */
static const ExpressionNode* first_return_expression(const ExternalDefinitionFunction* const function) {
    if (function == NULL)
        return NULL;
    const BlockItem* const item = function->body->as.compound_statement.head;
    if (item == NULL || item->kind != BLOCK_ITEM_KIND_STATEMENT || item->as.statement->kind != STATEMENT_KIND_RETURN)
        return NULL;
    return item->as.statement->as.return_statement.expression;
}

/**
 * Checks that @b fixture holds a program sema accepts: it parsed, sema reported no diagnostics, its table has one entry
 * per expression, and every entry has a type.
 * @return true iff all of that holds, so a test can go on to check single entries.
 */
static bool check_sema_passes(const SemaFixture* const fixture) {
    if (!CHECK(fixture->parsed))
        return false;
    bool passed = CHECK(list_get_size(fixture->context->diagnostic_report_table) == 0);
    passed = CHECK(fixture->result.expressions_count == fixture->expression_count) && passed;
    if (!CHECK(fixture->result.expressions_semantics != NULL || fixture->expression_count == 0))
        return false;
    for (int i = 0; i < fixture->result.expressions_count; i++)
        passed = CHECK(fixture->result.expressions_semantics[i].type != NULL) && passed;
    return passed;
}
/**
 * Checks that @b source parses, but sema rejects it: exactly one diagnostic, with code @b code at byte offset
 * @b offset .
 */
static void check_sema_fails(const char* const source, const DiagnosticCode code, const int offset) {
    const SemaFixture fixture = sema_fixture_create(source);

    if (CHECK(fixture.parsed)) {
        const List* const diagnostics = fixture.context->diagnostic_report_table;
        if (CHECK(list_get_size(diagnostics) == 1)) {
            const Diagnostic* const diagnostic = list_at_const(diagnostics, 0);
            CHECK(diagnostic->code == code);
            CHECK(diagnostic->range.position.offset == offset);
        }
    }

    sema_fixture_destroy(fixture);
}

// ---------------- Valid programs ---------------- //

/** "int main(void) { return 2; }": the slice 1 program. Its only expression, the constant 2, has type int. */
static void test_sema_slice_1_program(void) {
    const SemaFixture fixture = sema_fixture_create("int main(void) { return 2; }");

    if (check_sema_passes(&fixture)) {
        CHECK(fixture.result.expressions_count == 1);
        const ExpressionNode* const two = first_return_expression(function_at(fixture.ast, 0));
        if (CHECK(two != NULL)) {
            CHECK(semantics_of(&fixture, two)->type == type_get_int());
            CHECK(semantics_of(&fixture, two)->identifier_symbol == NULL);  // not an identifier
        }
    }

    sema_fixture_destroy(fixture);
}
/** "int f(void){return 1;} int g(void){return 2;}": two functions with different names. */
static void test_sema_two_functions(void) {
    const SemaFixture fixture = sema_fixture_create("int f(void){return 1;} int g(void){return 2;}");

    if (check_sema_passes(&fixture)) {
        CHECK(fixture.result.expressions_count == 2);
        for (int i = 0; i < 2; i++) {
            const ExpressionNode* const constant = first_return_expression(function_at(fixture.ast, i));
            if (CHECK(constant != NULL))
                CHECK(semantics_of(&fixture, constant)->type == type_get_int());
        }
    }

    sema_fixture_destroy(fixture);
}
/** Everything slice 1 has, together: several functions, nested and empty blocks, and returns inside blocks. */
static void test_sema_whole_program(void) {
    const SemaFixture fixture = sema_fixture_create(
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

    if (check_sema_passes(&fixture))
        CHECK(fixture.result.expressions_count == 3);

    sema_fixture_destroy(fixture);
}

// ---------------- Errors ---------------- //
// The expected offsets were counted by hand from the input, not copied from sema's output.

/**
 * "int f(void) { return 1; } int f(void) { return 2; }": a function defined twice (§6.9p5: at most one external
 * definition per name). Reported at the second definition's name, the 'f' at offset 30.
 */
static void test_sema_error_function_defined_twice(void) {
    check_sema_fails("int f(void) { return 1; } int f(void) { return 2; }",
                     DIAGNOSTIC_CODE_SEMA_SYMBOL_ALREADY_DEFINED, 30);
}
/**
 * "int f(void){return 1;} int f(void){return 2;} int f(void){return 3;}": sema stops at the first error (ADR-025), so
 * the third definition isn't reported too. Reported at the second 'f', offset 27.
 */
static void test_sema_error_function_defined_three_times(void) {
    check_sema_fails("int f(void){return 1;} int f(void){return 2;} int f(void){return 3;}",
                     DIAGNOSTIC_CODE_SEMA_SYMBOL_ALREADY_DEFINED, 27);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);

    RUN_TEST(test_sema_slice_1_program);
    RUN_TEST(test_sema_two_functions);
    RUN_TEST(test_sema_whole_program);

    RUN_TEST(test_sema_error_function_defined_twice);
    RUN_TEST(test_sema_error_function_defined_three_times);

    return harness_finish();
}
