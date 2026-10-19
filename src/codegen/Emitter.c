//
// Created by tomer on 19-Oct-26.
//

#include <assert.h>
#include <stdlib.h>

#include "Emitter.h"

#include "Text.h"
#include "TextView.h"
#include "Utils.h"

struct Emitter {
    int label_counter;
    int indentation_counter;
    Text* emissions;    ///< the total assembly output so far.
}; ///< append only, assembly stream (for instructions, labels, etc.)

static void emitter_emit_sections_headers(Emitter* const this);
Emitter* emitter_create(void) {
    Emitter* to_return = utils_xmalloc(sizeof (Emitter));
    *to_return = (Emitter){
        .label_counter = 0,
        .indentation_counter = 0,
        .emissions = text_create_empty()
    };

    emitter_emit_sections_headers(to_return);
    return to_return;
}
void emitter_destroy(                                   Emitter* const this) {
    assert(this != NULL);

    if (this->emissions != NULL) // might have been detached.
        text_destroy(this->emissions);
    free(this);
}

static void emitter_emit_separation(                    Emitter* const this) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    text_append_char(this->emissions, ',');
    text_append_char(this->emissions, ' ');
}
static void emitter_emit_indentations(                  Emitter* const this) {
    assert(this != NULL);
    assert(this->emissions != NULL);
    assert(this->indentation_counter >= 0);

    for (int i = 0; i < this->indentation_counter; ++i)
        text_append_text_view(this->emissions, text_view_value_of_literal("    "));
}
static void emitter_emit_new_line(                      Emitter* const this) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    text_append_char(this->emissions, '\n');
    emitter_emit_indentations(this);
}
static void emitter_emit_instruction_suffix(            Emitter* const this, const Width width) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    switch (width) {
        case WIDTH_BYTE:
            text_append_char(this->emissions, 'b');
            break;
        case WIDTH_LONG:
            text_append_char(this->emissions, 'l');
            break;
        case WIDTH_QUAD:
            text_append_char(this->emissions, 'q');
            break;
        default:
            assert(false && "invalid program state");
    }
}
static void emitter_emit_displacement(                  Emitter* const this, const Displacement displacement) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    switch (displacement.kind) {
        case DISPLACEMENT_KIND_LABEL: {
            Text* const label_spelling = label_get_spelling(displacement.as.label);
            text_append_text(this->emissions, label_spelling);
            text_destroy(label_spelling);
            break;
        }
        case DISPLACEMENT_KIND_NUMBER:
            text_append_int(this->emissions, displacement.as.number);
            break;
        case DISPLACEMENT_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void emitter_emit_register_name(                 Emitter* const this, const RegisterKind kind) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    switch (kind) {
        case REGISTER_KIND_RAX:
            text_append_char(this->emissions, 'a');
            break;
        case REGISTER_KIND_RBX:
            text_append_char(this->emissions, 'b');
            break;
        case REGISTER_KIND_RCX:
            text_append_char(this->emissions, 'c');
            break;
        case REGISTER_KIND_RDX:
            text_append_char(this->emissions, 'd');
            break;

        case REGISTER_KIND_RBP:
            text_append_text_view(this->emissions, text_view_value_of_literal("bp"));
            break;
        case REGISTER_KIND_RSI:
            text_append_text_view(this->emissions, text_view_value_of_literal("si"));
            break;
        case REGISTER_KIND_RDI:
            text_append_text_view(this->emissions, text_view_value_of_literal("di"));
            break;
        case REGISTER_KIND_RSP:
            text_append_text_view(this->emissions, text_view_value_of_literal("sp"));
            break;
        case REGISTER_KIND_RIP:
            text_append_text_view(this->emissions, text_view_value_of_literal("ip"));
            break;

        case REGISTER_KIND_R8:
            text_append_text_view(this->emissions, text_view_value_of_literal("r8"));
            break;
        case REGISTER_KIND_R9:
            text_append_text_view(this->emissions, text_view_value_of_literal("r9"));
            break;
        case REGISTER_KIND_R10:
            text_append_text_view(this->emissions, text_view_value_of_literal("r10"));
            break;
        case REGISTER_KIND_R11:
            text_append_text_view(this->emissions, text_view_value_of_literal("r11"));
            break;
        case REGISTER_KIND_R12:
            text_append_text_view(this->emissions, text_view_value_of_literal("r12"));
            break;
        case REGISTER_KIND_R13:
            text_append_text_view(this->emissions, text_view_value_of_literal("r13"));
            break;
        case REGISTER_KIND_R14:
            text_append_text_view(this->emissions, text_view_value_of_literal("r14"));
            break;
        case REGISTER_KIND_R15:
            text_append_text_view(this->emissions, text_view_value_of_literal("r15"));
            break;
        case REGISTER_KIND_NONE:
        case REGISTER_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void emitter_emit_operand_register(              Emitter* const this, const Register register_) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    text_append_char(this->emissions, '%');
    switch (register_.width) {
        case WIDTH_BYTE:
            emitter_emit_register_name(this, register_.kind);
            switch (register_.kind) {
                case REGISTER_KIND_RAX:
                case REGISTER_KIND_RBX:
                case REGISTER_KIND_RCX:
                case REGISTER_KIND_RDX:
                case REGISTER_KIND_RBP:
                case REGISTER_KIND_RSI:
                case REGISTER_KIND_RDI:
                case REGISTER_KIND_RSP:
                    text_append_char(this->emissions, 'l');
                    break;
                case REGISTER_KIND_R8:
                case REGISTER_KIND_R9:
                case REGISTER_KIND_R10:
                case REGISTER_KIND_R11:
                case REGISTER_KIND_R12:
                case REGISTER_KIND_R13:
                case REGISTER_KIND_R14:
                case REGISTER_KIND_R15:
                    text_append_char(this->emissions, 'b');
                    break;
                case REGISTER_KIND_RIP:
                case REGISTER_KIND_NONE:
                case REGISTER_KIND__COUNT:
                default:
                    assert(false && "invalid program state");
            }
            break;
        case WIDTH_LONG:
            switch (register_.kind) {
                case REGISTER_KIND_RAX:
                case REGISTER_KIND_RBX:
                case REGISTER_KIND_RCX:
                case REGISTER_KIND_RDX:
                    text_append_char(this->emissions, 'e');
                    emitter_emit_register_name(this, register_.kind);
                    text_append_char(this->emissions, 'x');
                    break;
                case REGISTER_KIND_RBP:
                case REGISTER_KIND_RSI:
                case REGISTER_KIND_RDI:
                case REGISTER_KIND_RSP:
                    text_append_char(this->emissions, 'e');
                    emitter_emit_register_name(this, register_.kind);
                    break;
                case REGISTER_KIND_R8:
                case REGISTER_KIND_R9:
                case REGISTER_KIND_R10:
                case REGISTER_KIND_R11:
                case REGISTER_KIND_R12:
                case REGISTER_KIND_R13:
                case REGISTER_KIND_R14:
                case REGISTER_KIND_R15:
                    emitter_emit_register_name(this, register_.kind);
                    text_append_char(this->emissions, 'd');
                    break;
                case REGISTER_KIND_RIP:
                case REGISTER_KIND_NONE:
                case REGISTER_KIND__COUNT:
                default:
                    assert(false && "invalid program state");
            }
            break;
        case WIDTH_QUAD:
            switch (register_.kind) {
                case REGISTER_KIND_RAX:
                case REGISTER_KIND_RBX:
                case REGISTER_KIND_RCX:
                case REGISTER_KIND_RDX:
                    text_append_char(this->emissions, 'r');
                    emitter_emit_register_name(this, register_.kind);
                    text_append_char(this->emissions, 'x');
                    break;
                case REGISTER_KIND_RBP:
                case REGISTER_KIND_RSI:
                case REGISTER_KIND_RDI:
                case REGISTER_KIND_RSP:
                case REGISTER_KIND_RIP:
                    text_append_char(this->emissions, 'r');
                    emitter_emit_register_name(this, register_.kind);
                    break;
                case REGISTER_KIND_R8:
                case REGISTER_KIND_R9:
                case REGISTER_KIND_R10:
                case REGISTER_KIND_R11:
                case REGISTER_KIND_R12:
                case REGISTER_KIND_R13:
                case REGISTER_KIND_R14:
                case REGISTER_KIND_R15:
                    emitter_emit_register_name(this, register_.kind);
                    break;
                case REGISTER_KIND_NONE:
                case REGISTER_KIND__COUNT:
                default:
                    assert(false && "invalid program state");
            }
            break;
        default:
            assert(false && "invalid program state");
            break;
    }
}
static void emitter_emit_operand_memory_sib(            Emitter* const this, const EffectiveAddressSib effective_address_sib) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    emitter_emit_displacement(this, effective_address_sib.displacement);
    text_append_char(this->emissions, '(');
    // Base
    emitter_emit_operand_register(this, effective_address_sib.base);
    if (effective_address_sib.index.kind != REGISTER_KIND_NONE) {
        emitter_emit_separation(this);
        // Index
        emitter_emit_operand_register(this, effective_address_sib.index);
        emitter_emit_separation(this);
        // Scale
        text_append_int(this->emissions, effective_address_sib.scale); // no leading '$', since it is not an immediate.
    }
    text_append_char(this->emissions, ')');
}
static void emitter_emit_operand_memory_rip_relative(   Emitter* const this, const EffectiveAddressRipRelative effective_address_rip_relative) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    emitter_emit_displacement(this, effective_address_rip_relative.displacement); // no leading '$', since it is not an immediate.
    text_append_char(this->emissions, '(');
    emitter_emit_operand_register(this, effective_address_rip_relative.rip);
    text_append_char(this->emissions, ')');
}
static void emitter_emit_operand_memory(                Emitter* const this, const EffectiveAddress effective_address) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    switch (effective_address.kind) {
        case EFFECTIVE_ADDRESS_KIND_SIB:
            emitter_emit_operand_memory_sib(this, effective_address.as.sib);
            break;
        case EFFECTIVE_ADDRESS_KIND_RIP_RELATIVE:
            emitter_emit_operand_memory_rip_relative(this, effective_address.as.rip_relative);
            break;
        case EFFECTIVE_ADDRESS_KIND__COUNT:
        default:
            assert(false && "invalid program state");
            break;
    }
}
static void emitter_emit_operand_immediate(             Emitter* const this, const int immediate) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    text_append_char(this->emissions, '$');
    text_append_int(this->emissions, immediate);
}
static void emitter_emit_operand(                       Emitter* const this, const Operand operand) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    switch (operand.kind) {
        case OPERAND_KIND_REGISTER:
            emitter_emit_operand_register(this, operand.as.register_);
            break;
        case OPERAND_KIND_MEMORY:
            emitter_emit_operand_memory(this, operand.as.memory);
            break;
        case OPERAND_KIND_IMMEDIATE:
            emitter_emit_operand_immediate(this, operand.as.immediate);
            break;
        case OPERAND_KIND__COUNT:
        default:
            assert(false && "invalid program state");
    }
}
static void emitter_emit_sections_headers(              Emitter* const this) {
    text_append_text_view(this->emissions, text_view_value_of_literal(".text"));
    emitter_emit_new_line(this);
    text_append_text_view(this->emissions, text_view_value_of_literal(".globl main"));
    emitter_emit_new_line(this);
    text_append_text_view(this->emissions, text_view_value_of_literal(".text"));
    emitter_emit_new_line(this);
}
static void emitter_emit_footers(                       Emitter* const this) {
    emitter_emit_new_line(this);
    text_append_text_view(this->emissions, text_view_value_of_literal(".section .note.GNU-stack,\"\",@progbits"));
    emitter_emit_new_line(this);
}

void emitter_indent(Emitter* const this) {
    assert(this != NULL);

    this->indentation_counter++;
}
void emitter_dedent(Emitter* const this) {
    assert(this != NULL);

    this->indentation_counter--;
}
Text* emitter_detach_text(Emitter* const this) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    assert(this->indentation_counter == 0);
    emitter_emit_footers(this);

    Text* const to_return = this->emissions;
    this->emissions = NULL;
    return to_return;
}
void emitter_emit_label(Emitter* const this, const Label label) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    emitter_emit_new_line(this);

    // e.g.: ".L_1_while:"
    Text* const spelling = label_get_spelling(label);
    text_append_text(this->emissions, spelling);
    text_append_char(this->emissions, ':');
    text_destroy(spelling);
}
/**
 * @note: look at the source code. The builtin Doxygen screws the table formats.
 *
 * A quick reference to the different operands combinations:
 * s\d| M | R | I
 * ---+---+---+---
 *  M | X | * | X
 * ---+---+---+---
 *  R | * | * | X
 * ---+---+---+---
 *  I | **| * | X
 *
 *  M: memory operand; R: register operand; I: immediate operand.
 *  X: invalid combination
 *  *: provided withs must match the registers ones.
 *  **: uses the provided width as-is.
 *
 *  the various asserts will have numbers next to them. That corresponds to the operand-combination-options:
 * s\d| M | R | I
 * ---+---+---+---
 *  M | 1 | 2 | 3
 * ---+---+---+---
 *  R | 4 | 5 | 6
 * ---+---+---+---
 *  I | 7 | 8 | 9
 */
void emitter_emit_mov(Emitter* const this, const Width width, const Operand source, const Operand destination) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    assert(destination.kind != OPERAND_KIND_IMMEDIATE);                                             // #3, #6, #9
    assert(!(source.kind == OPERAND_KIND_MEMORY && destination.kind == OPERAND_KIND_MEMORY));       // #1
    assert(source.kind      != OPERAND_KIND_REGISTER || source.as.register_.width       == width);  // #4, #5
    assert(destination.kind != OPERAND_KIND_REGISTER || destination.as.register_.width  == width);  // #2, #5, #8

    emitter_emit_new_line(this);

    // e.g.: "movq "
    text_append_text_view(this->emissions, text_view_value_of_literal("mov"));
    emitter_emit_instruction_suffix(this, width);
    text_append_char(this->emissions, ' ');

    // e.g.: "$5, %rax"
    emitter_emit_operand(this, source);
    emitter_emit_separation(this);
    emitter_emit_operand(this, destination);
}
void emitter_emit_ret(Emitter* const this) {
    assert(this != NULL);
    assert(this->emissions != NULL);

    emitter_emit_new_line(this);
    text_append_text_view(this->emissions, text_view_value_of_literal("ret"));
}

