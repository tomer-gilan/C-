//
// Written by Claude on 19-Oct-26.
//
// Unit tests for Operand.h: every constructor keeps what it's given, and label_get_spelling spells both label kinds.
// The constructors' asserts (e.g. %rsp as an index) abort the program, which the harness can't check, so they aren't
// tested here.
//

#include <stdio.h>
#include <string.h>

#include "Context.h"
#include "Harness.h"
#include "Operand.h"
#include "SourceText.h"
#include "Text.h"
#include "TextView.h"

/** The compiler's arguments for a test, as if run as `c_compiler <source> <output>`. Tests never write the output. */
static CompilationArguments test_compilation_arguments(void) {
    static const char* const argv[] = {"c_compiler", "<source>", "<unused>.s"};
    return compilation_flags_from_arguments((int)(sizeof argv / sizeof argv[0]), argv);
}
/** A context whose identifiers table starts empty; label_make_symbol reads spellings from it. */
static Context* operand_context_create(void) {
    return context_create(source_text_create(text_create_from_buffer("main", 4), "<source>"),
                          test_compilation_arguments());
}

/** Compares @b text with @b expected , prints both on a mismatch, then destroys @b text . */
static bool text_spells_then_destroy(Text* const text, const char* const expected) {
    const bool equal = text_view_equals(text_as_view(text), text_view_make((int)strlen(expected), expected));
    if (!equal)
        fprintf(stderr, "    expected \"%s\", got \"%.*s\"\n", expected, text->length, text->data);
    text_destroy(text);
    return equal;
}

static bool registers_equal(const Register first, const Register second) {
    return first.kind == second.kind && first.width == second.width;
}

// -------- Label -------- //

static void test_label_make_auto_keeps_id_and_name(void) {
    const Label label = label_make_auto(3, text_view_value_of_literal("while"));

    CHECK(label.kind == LABEL_KIND_AUTO);
    CHECK(label.as.label_auto.auto_id == 3);
    CHECK(text_view_equals(label.as.label_auto.name, text_view_value_of_literal("while")));
}
/** The spelling comes from the identifier's own entry, not from the first one in the table. */
static void test_label_make_symbol_takes_spelling_from_identifiers_table(void) {
    Context* const context = operand_context_create();
    context_intern_identifier(context, text_view_value_of_literal("f"));
    const int main_id = context_intern_identifier(context, text_view_value_of_literal("main"));

    const Label label = label_make_symbol(context, main_id);

    CHECK(label.kind == LABEL_KIND_SYMBOL);
    CHECK(label.as.label_symbol.identifier_id == main_id);
    CHECK(text_view_equals(label.as.label_symbol.spelling, text_view_value_of_literal("main")));

    context_destroy(context);
}
/** Auto labels are spelled .L_<id>_<name>: the .L prefix keeps them out of the object file's symbol table. */
static void test_label_spelling_of_auto_label(void) {
    CHECK(text_spells_then_destroy(label_get_spelling(label_make_auto(0, text_view_value_of_literal("end"))),
                                   ".L_0_end"));
    CHECK(text_spells_then_destroy(label_get_spelling(label_make_auto(12, text_view_value_of_literal("while"))),
                                   ".L_12_while"));
}
/** A symbol label is the C name as is, so the linker can match it across files. */
static void test_label_spelling_of_symbol_label(void) {
    Context* const context = operand_context_create();
    const int main_id = context_intern_identifier(context, text_view_value_of_literal("main"));

    CHECK(text_spells_then_destroy(label_get_spelling(label_make_symbol(context, main_id)), "main"));

    context_destroy(context);
}

// -------- Displacement -------- //

static void test_displacement_make_number_keeps_number(void) {
    const Displacement positive = displacement_make_number(16);
    const Displacement negative = displacement_make_number(-8);

    CHECK(positive.kind == DISPLACEMENT_KIND_NUMBER);
    CHECK(positive.as.number == 16);
    CHECK(negative.kind == DISPLACEMENT_KIND_NUMBER);
    CHECK(negative.as.number == -8);
}
static void test_displacement_make_label_keeps_label(void) {
    const Displacement displacement = displacement_make_label(label_make_auto(2, text_view_value_of_literal("str")));

    CHECK(displacement.kind == DISPLACEMENT_KIND_LABEL);
    CHECK(displacement.as.label.kind == LABEL_KIND_AUTO);
    CHECK(displacement.as.label.as.label_auto.auto_id == 2);
    CHECK(text_view_equals(displacement.as.label.as.label_auto.name, text_view_value_of_literal("str")));
}

// -------- Register -------- //

static void test_register_make_keeps_kind_and_width(void) {
    const Register rax = register_make(REGISTER_KIND_RAX, WIDTH_QUAD);
    const Register r15d = register_make(REGISTER_KIND_R15, WIDTH_LONG);
    const Register sil = register_make(REGISTER_KIND_RSI, WIDTH_BYTE);

    CHECK(rax.kind == REGISTER_KIND_RAX && rax.width == WIDTH_QUAD);
    CHECK(r15d.kind == REGISTER_KIND_R15 && r15d.width == WIDTH_LONG);
    CHECK(sil.kind == REGISTER_KIND_RSI && sil.width == WIDTH_BYTE);
}
/** Zero-initialized registers mean "no register" (an absent SIB index). */
static void test_register_kind_none_is_zero(void) {
    const Register zeroed = {0};

    CHECK(REGISTER_KIND_NONE == 0);
    CHECK(zeroed.kind == REGISTER_KIND_NONE);
}

// -------- EffectiveAddress -------- //

/** The parameters come in SIB-byte order (scale, index, base); the fields must not get swapped on the way in. */
static void test_effective_address_make_sib_keeps_parts(void) {
    const Register base = register_make(REGISTER_KIND_RAX, WIDTH_QUAD);
    const Register index_register = register_make(REGISTER_KIND_RCX, WIDTH_QUAD);

    const EffectiveAddress address =
        effective_address_make_sib(WIDTH_LONG, index_register, base, displacement_make_number(16));

    CHECK(address.kind == EFFECTIVE_ADDRESS_KIND_SIB);
    CHECK(registers_equal(address.as.sib.base, base));
    CHECK(registers_equal(address.as.sib.index, index_register));
    CHECK(address.as.sib.scale == WIDTH_LONG);
    CHECK(address.as.sib.displacement.kind == DISPLACEMENT_KIND_NUMBER);
    CHECK(address.as.sib.displacement.as.number == 16);
}
/** The common stack-slot form, -8(%rbp): a base and no index. */
static void test_effective_address_make_sib_without_index(void) {
    const Register rbp = register_make(REGISTER_KIND_RBP, WIDTH_QUAD);
    const Register none = register_make(REGISTER_KIND_NONE, WIDTH_QUAD);

    const EffectiveAddress address = effective_address_make_sib(WIDTH_BYTE, none, rbp, displacement_make_number(-8));

    CHECK(address.kind == EFFECTIVE_ADDRESS_KIND_SIB);
    CHECK(registers_equal(address.as.sib.base, rbp));
    CHECK(address.as.sib.index.kind == REGISTER_KIND_NONE);
    CHECK(address.as.sib.displacement.as.number == -8);
}
/** RIP-relative addressing always goes through the full 64-bit %rip. */
static void test_effective_address_make_rip_relative_uses_rip(void) {
    Context* const context = operand_context_create();
    const int x_id = context_intern_identifier(context, text_view_value_of_literal("x"));

    const EffectiveAddress address =
        effective_address_make_rip_relative(displacement_make_label(label_make_symbol(context, x_id)));

    CHECK(address.kind == EFFECTIVE_ADDRESS_KIND_RIP_RELATIVE);
    CHECK(registers_equal(address.as.rip_relative.rip, register_make(REGISTER_KIND_RIP, WIDTH_QUAD)));
    CHECK(address.as.rip_relative.displacement.kind == DISPLACEMENT_KIND_LABEL);
    CHECK(address.as.rip_relative.displacement.as.label.kind == LABEL_KIND_SYMBOL);
    CHECK(address.as.rip_relative.displacement.as.label.as.label_symbol.identifier_id == x_id);

    context_destroy(context);
}

// -------- Operand -------- //

static void test_operand_make_register_keeps_register(void) {
    const Register eax = register_make(REGISTER_KIND_RAX, WIDTH_LONG);

    const Operand operand = operand_make_register(eax);

    CHECK(operand.kind == OPERAND_KIND_REGISTER);
    CHECK(registers_equal(operand.as.register_, eax));
}
static void test_operand_make_memory_keeps_address(void) {
    const Register rbp = register_make(REGISTER_KIND_RBP, WIDTH_QUAD);
    const Register none = register_make(REGISTER_KIND_NONE, WIDTH_QUAD);

    const Operand operand =
        operand_make_memory(effective_address_make_sib(WIDTH_BYTE, none, rbp, displacement_make_number(-4)));

    CHECK(operand.kind == OPERAND_KIND_MEMORY);
    CHECK(operand.as.memory.kind == EFFECTIVE_ADDRESS_KIND_SIB);
    CHECK(registers_equal(operand.as.memory.as.sib.base, rbp));
    CHECK(operand.as.memory.as.sib.displacement.as.number == -4);
}
static void test_operand_make_immediate_keeps_value(void) {
    const Operand positive = operand_make_immediate(42);
    const Operand negative = operand_make_immediate(-1);

    CHECK(positive.kind == OPERAND_KIND_IMMEDIATE);
    CHECK(positive.as.immediate == 42);
    CHECK(negative.kind == OPERAND_KIND_IMMEDIATE);
    CHECK(negative.as.immediate == -1);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);

    RUN_TEST(test_label_make_auto_keeps_id_and_name);
    RUN_TEST(test_label_make_symbol_takes_spelling_from_identifiers_table);
    RUN_TEST(test_label_spelling_of_auto_label);
    RUN_TEST(test_label_spelling_of_symbol_label);

    RUN_TEST(test_displacement_make_number_keeps_number);
    RUN_TEST(test_displacement_make_label_keeps_label);

    RUN_TEST(test_register_make_keeps_kind_and_width);
    RUN_TEST(test_register_kind_none_is_zero);

    RUN_TEST(test_effective_address_make_sib_keeps_parts);
    RUN_TEST(test_effective_address_make_sib_without_index);
    RUN_TEST(test_effective_address_make_rip_relative_uses_rip);

    RUN_TEST(test_operand_make_register_keeps_register);
    RUN_TEST(test_operand_make_memory_keeps_address);
    RUN_TEST(test_operand_make_immediate_keeps_value);

    return harness_finish();
}
