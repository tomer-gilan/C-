//
// Created by tomer on 16-Oct-26.
//

#ifndef C_COMPILER_TYPE_H
#define C_COMPILER_TYPE_H
#include <stdbool.h>

#include "Array.h"

typedef struct Type Type;

extern const int CHAR_TYPE_SIZE;
extern const int INT_TYPE_SIZE;
extern const int POINTER_TYPE_SIZE;

typedef struct {
    const Type* type;
} TypePointer;
typedef struct {
    const Type* type;
    int length;
} TypeArray;
typedef struct {
    const Type* return_type;
    const Type* const* parameters_type;
    int parameters_count;
} TypeFunction;

typedef enum {
    TYPE_KIND_INT,
    TYPE_KIND_CHAR,
    TYPE_KIND_VOID,

    TYPE_KIND_POINTER,
    TYPE_KIND_ARRAY,
    TYPE_KIND_FUNCTION,
} TypeKind;
struct Type {
    TypeKind kind;
    union {
        TypePointer pointer;
        TypeArray array;
        TypeFunction function;
    } as;
};

bool type_are_equal(const Type* first, const Type* second);
int type_get_size(const Type* this);
const Type* type_get_singleton(TypeKind kind);
const Type* type_get_void(void);
const Type* type_get_char(void);
const Type* type_get_int(void);

#endif //C_COMPILER_TYPE_H
