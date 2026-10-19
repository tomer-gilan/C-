//
// Created by tomer on 19-Oct-26.
//

#ifndef C_COMPILER_OPERAND_H
#define C_COMPILER_OPERAND_H
#include "Context.h"
#include "TextView.h"

// Label:
typedef enum {
    LABEL_KIND_AUTO,    ///< e.g. ".L_1_count:"
    LABEL_KIND_SYMBOL,  ///< e.g. "main:"

    LABEL_KIND__COUNT,
} LabelKind;
typedef struct {
    int auto_id;
    TextView name;
} LabelAuto;
typedef struct {
    int identifier_id;
    TextView spelling;
} LabelSymbol;
typedef struct {
    LabelKind kind;
    union {
        LabelAuto label_auto;
        LabelSymbol label_symbol;
    } as;
} Label;

Label label_make_auto(int auto_id, TextView name);
Label label_make_symbol(const Context* context, int identifier_id);

Text* label_get_spelling(Label this);

typedef enum {
    DISPLACEMENT_KIND_LABEL,
    DISPLACEMENT_KIND_NUMBER,

    DISPLACEMENT_KIND__COUNT,
} DisplacementKind;
typedef struct {
    DisplacementKind kind;
    union {
        Label label;
        int number;
    } as;
} Displacement;

Displacement displacement_make_number(int number);
Displacement displacement_make_label(Label label);

// Register:
typedef enum {
    WIDTH_BYTE = 1, ///< 8bit,  AT&T suffix @b b
    WIDTH_LONG = 4, ///< 32bit, AT&T suffix @b l
    WIDTH_QUAD = 8, ///< 64bit, AT&T suffix @b q
} Width;
typedef enum {
    REGISTER_KIND_NONE,

    REGISTER_KIND_RAX,
    REGISTER_KIND_RBX,
    REGISTER_KIND_RCX,
    REGISTER_KIND_RDX,

    REGISTER_KIND_RBP,
    REGISTER_KIND_RSI,
    REGISTER_KIND_RDI,
    REGISTER_KIND_RSP,

    REGISTER_KIND_R8,
    REGISTER_KIND_R9,
    REGISTER_KIND_R10,
    REGISTER_KIND_R11,
    REGISTER_KIND_R12,
    REGISTER_KIND_R13,
    REGISTER_KIND_R14,
    REGISTER_KIND_R15,

    REGISTER_KIND_RIP,

    REGISTER_KIND__COUNT,
} RegisterKind;
typedef struct {
    RegisterKind kind;
    Width width;
} Register;

Register register_make(RegisterKind kind, Width width);

// EffectiveAddress:
typedef enum {
    EFFECTIVE_ADDRESS_KIND_SIB,
    EFFECTIVE_ADDRESS_KIND_RIP_RELATIVE,

    EFFECTIVE_ADDRESS_KIND__COUNT
} EffectiveAddressKind;
typedef struct {
    Register base;
    Register index;
    Width scale;
    Displacement displacement;
} EffectiveAddressSib;
typedef struct {
    Displacement displacement;
    Register rip;
} EffectiveAddressRipRelative;
typedef struct {
    EffectiveAddressKind kind;
    union {
        EffectiveAddressSib sib;
        EffectiveAddressRipRelative rip_relative;
    } as; ///< keyed by @c EffectiveAddressKind

} EffectiveAddress;

EffectiveAddress effective_address_make_sib(Width scale, Register index, Register base, Displacement displacement);
EffectiveAddress effective_address_make_rip_relative(Displacement displacement);

// Operand:
typedef enum {
    OPERAND_KIND_REGISTER,
    OPERAND_KIND_MEMORY,
    OPERAND_KIND_IMMEDIATE,

    OPERAND_KIND__COUNT
} OperandKind;
typedef struct {
    OperandKind kind;
    union {
        Register register_;
        EffectiveAddress memory;
        int immediate;
    } as; ///< keyed by @c OperandKind
} Operand;

Operand operand_make_register(Register register_);
Operand operand_make_memory(EffectiveAddress effective_address);
Operand operand_make_immediate(int immediate);

#endif //C_COMPILER_OPERAND_H
