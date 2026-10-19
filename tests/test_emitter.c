//
// Written by Claude on 19-Oct-26.
//
// Unit tests for Emitter.h: the exact AT&T text each emit call appends. Every emit starts its own line, labels are
// indented like instructions, and the output is wrapped in the file's header (emitter_create) and footer (detaching).
// Each test's expected string holds only the lines between the two; emitter_output_is adds them. Expected spellings
// are the ones GNU as accepts. The emitter's asserts (e.g. a memory-to-memory mov) abort the program, which the harness
// can't check, so they aren't tested here.
//

#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "Context.h"
#include "Emitter.h"
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
/** A context whose identifiers table starts empty; symbol labels read their spellings from it. */
static Context* emitter_context_create(void) {
    return context_create(source_text_create(text_create_from_buffer("main", 4), "<source>"),
                          test_compilation_arguments());
}

/** What every output starts with: the code section, and main made visible to the linker. */
static const TextView output_header = TEXT_VIEW_INITIALIZER_OF_LITERAL(".text\n.globl main\n.text\n");
/**
 * What every output ends with: an empty .note.GNU-stack section, so the linker marks the stack non-executable. Spelled
 * the way gcc -S spells it: the flags argument is the empty string "", quotes included.
 */
static const TextView output_footer = TEXT_VIEW_INITIALIZER_OF_LITERAL(".section .note.GNU-stack,\"\",@progbits\n");

/**
 * Detaches the output of @b emitter and compares it with @b expected_lines between the header and the footer. Prints
 * both on a mismatch. Destroys @b emitter and the detached text.
 * @param expected_lines the emitted lines: each starts with its own "\n", and the last one ends with one more.
 */
static bool emitter_output_is(Emitter* const emitter, const char* const expected_lines) {
    Text* const output = emitter_detach_text(emitter);
    emitter_destroy(emitter);

    Text* const expected = text_create_from_view(output_header);
    text_append_text_view(expected, text_view_make((int)strlen(expected_lines), expected_lines));
    text_append_text_view(expected, output_footer);

    const bool equal = text_equals(output, expected);
    if (!equal)
        fprintf(stderr, "    expected:\n\"%.*s\"\n    got:\n\"%.*s\"\n", expected->length, expected->data,
                output->length, output->data);
    text_destroy(expected);
    text_destroy(output);
    return equal;
}

static Operand register_operand(const RegisterKind kind, const Width width) {
    return operand_make_register(register_make(kind, width));
}
/** disp(%base): a base register and no index. */
static Operand base_operand(const RegisterKind base, const int displacement) {
    return operand_make_memory(effective_address_make_sib(WIDTH_BYTE, register_make(REGISTER_KIND_NONE, WIDTH_QUAD),
                                                          register_make(base, WIDTH_QUAD),
                                                          displacement_make_number(displacement)));
}

// -------- Lines and indentation -------- //

/** Nothing emitted: detaching still ends the output with a newline. */
static void test_emitter_empty_output_is_one_newline(void) {
    CHECK(emitter_output_is(emitter_create(), "\n"));
}
static void test_emitter_ret(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_ret(emitter);

    CHECK(emitter_output_is(emitter, "\nret\n"));
}
/** Each instruction gets its own line: two in a row must not run together. */
static void test_emitter_instructions_on_separate_lines(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(1), register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(2), register_operand(REGISTER_KIND_RCX, WIDTH_LONG));
    emitter_emit_ret(emitter);

    CHECK(emitter_output_is(emitter, "\nmovl $1, %eax\nmovl $2, %ecx\nret\n"));
}
/** The slice 1 shape: a function label at level 0, its body one level in. */
static void test_emitter_function_body_is_indented(void) {
    Context* const context = emitter_context_create();
    const int main_id = context_intern_identifier(context, text_view_value_of_literal("main"));
    Emitter* const emitter = emitter_create();

    emitter_emit_label(emitter, label_make_symbol(context, main_id));
    emitter_indent(emitter);
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(0), register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_emit_ret(emitter);
    emitter_dedent(emitter);

    CHECK(emitter_output_is(emitter, "\nmain:\n    movl $0, %eax\n    ret\n"));
    context_destroy(context);
}
/** A second function's label starts its own line after the first one's ret. */
static void test_emitter_two_functions(void) {
    Context* const context = emitter_context_create();
    const int f_id = context_intern_identifier(context, text_view_value_of_literal("f"));
    const int main_id = context_intern_identifier(context, text_view_value_of_literal("main"));
    Emitter* const emitter = emitter_create();

    emitter_emit_label(emitter, label_make_symbol(context, f_id));
    emitter_indent(emitter);
    emitter_emit_ret(emitter);
    emitter_dedent(emitter);
    emitter_emit_label(emitter, label_make_symbol(context, main_id));
    emitter_indent(emitter);
    emitter_emit_ret(emitter);
    emitter_dedent(emitter);

    CHECK(emitter_output_is(emitter, "\nf:\n    ret\nmain:\n    ret\n"));
    context_destroy(context);
}
/** A label is printed at the current level, and opens the next one when the caller indents after it. */
static void test_emitter_nested_indentation(void) {
    Context* const context = emitter_context_create();
    const int main_id = context_intern_identifier(context, text_view_value_of_literal("main"));
    Emitter* const emitter = emitter_create();

    emitter_emit_label(emitter, label_make_symbol(context, main_id));
    emitter_indent(emitter);
    emitter_emit_label(emitter, label_make_auto(1, text_view_value_of_literal("while")));
    emitter_indent(emitter);
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(1), register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_dedent(emitter);
    emitter_emit_ret(emitter);
    emitter_dedent(emitter);

    CHECK(emitter_output_is(emitter, "\nmain:\n    .L_1_while:\n        movl $1, %eax\n    ret\n"));
    context_destroy(context);
}

// -------- Labels -------- //

static void test_emitter_symbol_label(void) {
    Context* const context = emitter_context_create();
    const int main_id = context_intern_identifier(context, text_view_value_of_literal("main"));
    Emitter* const emitter = emitter_create();
    emitter_emit_label(emitter, label_make_symbol(context, main_id));

    CHECK(emitter_output_is(emitter, "\nmain:\n"));
    context_destroy(context);
}
static void test_emitter_auto_label(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_label(emitter, label_make_auto(7, text_view_value_of_literal("end")));

    CHECK(emitter_output_is(emitter, "\n.L_7_end:\n"));
}

// -------- Registers -------- //

typedef struct {
    RegisterKind kind;
    const char* byte;
    const char* long_;
    const char* quad;
} RegisterSpelling;

/** All 16 general-purpose registers at each width (Intel SDM vol. 1, §3.4.1.1). */
static const RegisterSpelling register_spellings[] = {
    {REGISTER_KIND_RAX, "%al",   "%eax",  "%rax"},
    {REGISTER_KIND_RBX, "%bl",   "%ebx",  "%rbx"},
    {REGISTER_KIND_RCX, "%cl",   "%ecx",  "%rcx"},
    {REGISTER_KIND_RDX, "%dl",   "%edx",  "%rdx"},
    {REGISTER_KIND_RBP, "%bpl",  "%ebp",  "%rbp"},
    {REGISTER_KIND_RSI, "%sil",  "%esi",  "%rsi"},
    {REGISTER_KIND_RDI, "%dil",  "%edi",  "%rdi"},
    {REGISTER_KIND_RSP, "%spl",  "%esp",  "%rsp"},
    {REGISTER_KIND_R8,  "%r8b",  "%r8d",  "%r8"},
    {REGISTER_KIND_R9,  "%r9b",  "%r9d",  "%r9"},
    {REGISTER_KIND_R10, "%r10b", "%r10d", "%r10"},
    {REGISTER_KIND_R11, "%r11b", "%r11d", "%r11"},
    {REGISTER_KIND_R12, "%r12b", "%r12d", "%r12"},
    {REGISTER_KIND_R13, "%r13b", "%r13d", "%r13"},
    {REGISTER_KIND_R14, "%r14b", "%r14d", "%r14"},
    {REGISTER_KIND_R15, "%r15b", "%r15d", "%r15"},
};

/** Emits "mov<suffix> %reg, %reg" for one register at one width, and checks the line. */
static bool emitter_spells_register(const RegisterKind kind, const Width width, const char suffix,
                                    const char* const spelling) {
    Emitter* const emitter = emitter_create();
    emitter_emit_mov(emitter, width, register_operand(kind, width), register_operand(kind, width));

    char expected[64];
    snprintf(expected, sizeof expected, "\nmov%c %s, %s\n", suffix, spelling, spelling);
    return emitter_output_is(emitter, expected);
}
/** Every register at every width, as both source and destination; the width suffix follows the width. */
static void test_emitter_register_spellings(void) {
    const int count = (int)(sizeof register_spellings / sizeof register_spellings[0]);
    for (int i = 0; i < count; ++i) {
        const RegisterSpelling spelling = register_spellings[i];
        CHECK(emitter_spells_register(spelling.kind, WIDTH_BYTE, 'b', spelling.byte));
        CHECK(emitter_spells_register(spelling.kind, WIDTH_LONG, 'l', spelling.long_));
        CHECK(emitter_spells_register(spelling.kind, WIDTH_QUAD, 'q', spelling.quad));
    }
}

// -------- Immediates -------- //

static void test_emitter_immediates(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(0), register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(42), register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(-1), register_operand(REGISTER_KIND_RAX, WIDTH_LONG));

    CHECK(emitter_output_is(emitter, "\nmovl $0, %eax\nmovl $42, %eax\nmovl $-1, %eax\n"));
}
/** The ends of int's range, which an immediate operand can hold. */
static void test_emitter_immediate_limits(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(INT_MAX),
                     register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(INT_MIN),
                     register_operand(REGISTER_KIND_RAX, WIDTH_LONG));

    CHECK(emitter_output_is(emitter, "\nmovl $2147483647, %eax\nmovl $-2147483648, %eax\n"));
}

// -------- Memory operands -------- //

/** disp(%base): no index, so neither ", index, scale" nor a trailing comma. */
static void test_emitter_memory_base_only(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_mov(emitter, WIDTH_LONG, base_operand(REGISTER_KIND_RBP, -8),
                     register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_emit_mov(emitter, WIDTH_QUAD, base_operand(REGISTER_KIND_RDI, 0),
                     register_operand(REGISTER_KIND_RAX, WIDTH_QUAD));
    emitter_emit_mov(emitter, WIDTH_QUAD, base_operand(REGISTER_KIND_R12, 16),
                     register_operand(REGISTER_KIND_RAX, WIDTH_QUAD));

    CHECK(emitter_output_is(emitter, "\nmovl -8(%rbp), %eax\nmovq 0(%rdi), %rax\nmovq 16(%r12), %rax\n"));
}
/** disp(%base, %index, scale), at every scale the subset uses. */
static void test_emitter_memory_base_index_scale(void) {
    const Register rax = register_make(REGISTER_KIND_RAX, WIDTH_QUAD);
    const Register rcx = register_make(REGISTER_KIND_RCX, WIDTH_QUAD);
    const Width scales[] = {WIDTH_BYTE, WIDTH_LONG, WIDTH_QUAD};
    const char* const expected_lines[] = {
        "\nmovl 16(%rax, %rcx, 1), %edx\n",
        "\nmovl 16(%rax, %rcx, 4), %edx\n",
        "\nmovl 16(%rax, %rcx, 8), %edx\n",
    };

    for (int i = 0; i < 3; ++i) {
        Emitter* const emitter = emitter_create();
        const EffectiveAddress address = effective_address_make_sib(scales[i], rcx, rax, displacement_make_number(16));
        emitter_emit_mov(emitter, WIDTH_LONG, operand_make_memory(address),
                         register_operand(REGISTER_KIND_RDX, WIDTH_LONG));

        CHECK(emitter_output_is(emitter, expected_lines[i]));
    }
}
/** Globals and string literals: a label as the displacement, relative to %rip. */
static void test_emitter_memory_rip_relative(void) {
    Context* const context = emitter_context_create();
    const int x_id = context_intern_identifier(context, text_view_value_of_literal("x"));
    Emitter* const emitter = emitter_create();

    const Label string_label = label_make_auto(2, text_view_value_of_literal("str"));
    const Operand global = operand_make_memory(
        effective_address_make_rip_relative(displacement_make_label(label_make_symbol(context, x_id))));
    const Operand literal =
        operand_make_memory(effective_address_make_rip_relative(displacement_make_label(string_label)));
    emitter_emit_mov(emitter, WIDTH_LONG, global, register_operand(REGISTER_KIND_RAX, WIDTH_LONG));
    emitter_emit_mov(emitter, WIDTH_BYTE, literal, register_operand(REGISTER_KIND_RCX, WIDTH_BYTE));

    CHECK(emitter_output_is(emitter, "\nmovl x(%rip), %eax\nmovb .L_2_str(%rip), %cl\n"));
    context_destroy(context);
}

// -------- mov operand combinations -------- //

/** Register to memory: the width comes from the register. */
static void test_emitter_mov_register_to_memory(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_mov(emitter, WIDTH_QUAD, register_operand(REGISTER_KIND_RDI, WIDTH_QUAD),
                     base_operand(REGISTER_KIND_RBP, -16));
    emitter_emit_mov(emitter, WIDTH_BYTE, register_operand(REGISTER_KIND_RSI, WIDTH_BYTE),
                     base_operand(REGISTER_KIND_RBP, -1));

    CHECK(emitter_output_is(emitter, "\nmovq %rdi, -16(%rbp)\nmovb %sil, -1(%rbp)\n"));
}
/** Immediate to memory: no register to take the width from, so the given width alone picks the suffix. */
static void test_emitter_mov_immediate_to_memory(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_mov(emitter, WIDTH_BYTE, operand_make_immediate(7), base_operand(REGISTER_KIND_RBP, -1));
    emitter_emit_mov(emitter, WIDTH_LONG, operand_make_immediate(7), base_operand(REGISTER_KIND_RBP, -4));
    emitter_emit_mov(emitter, WIDTH_QUAD, operand_make_immediate(-1), base_operand(REGISTER_KIND_RBP, -8));

    CHECK(emitter_output_is(emitter, "\nmovb $7, -1(%rbp)\nmovl $7, -4(%rbp)\nmovq $-1, -8(%rbp)\n"));
}

// -------- Ownership -------- //

/** Destroying an emitter that was never detached frees its output too (LeakSanitizer checks this). */
static void test_emitter_destroy_without_detach(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_ret(emitter);
    emitter_destroy(emitter);

    CHECK(true);
}
/** The detached text belongs to the caller and outlives the emitter (AddressSanitizer checks the access). */
static void test_emitter_detached_text_outlives_emitter(void) {
    Emitter* const emitter = emitter_create();
    emitter_emit_ret(emitter);
    Text* const output = emitter_detach_text(emitter);
    emitter_destroy(emitter);

    // Reads the text's buffer after the emitter is gone. The last character: as warns about a file without a final newline.
    CHECK(output->length > 0 && output->data[output->length - 1] == '\n');
    text_destroy(output);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);

    RUN_TEST(test_emitter_empty_output_is_one_newline);
    RUN_TEST(test_emitter_ret);
    RUN_TEST(test_emitter_instructions_on_separate_lines);
    RUN_TEST(test_emitter_function_body_is_indented);
    RUN_TEST(test_emitter_two_functions);
    RUN_TEST(test_emitter_nested_indentation);

    RUN_TEST(test_emitter_symbol_label);
    RUN_TEST(test_emitter_auto_label);

    RUN_TEST(test_emitter_register_spellings);

    RUN_TEST(test_emitter_immediates);
    RUN_TEST(test_emitter_immediate_limits);

    RUN_TEST(test_emitter_memory_base_only);
    RUN_TEST(test_emitter_memory_base_index_scale);
    RUN_TEST(test_emitter_memory_rip_relative);

    RUN_TEST(test_emitter_mov_register_to_memory);
    RUN_TEST(test_emitter_mov_immediate_to_memory);

    RUN_TEST(test_emitter_destroy_without_detach);
    RUN_TEST(test_emitter_detached_text_outlives_emitter);

    return harness_finish();
}
