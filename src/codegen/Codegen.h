//
// Created by tomer on 19-Oct-26.
//

#ifndef C_COMPILER_CODEGEN_H
#define C_COMPILER_CODEGEN_H
#include "Emitter.h"
#include "Sema.h"

typedef struct {
    Context* context;               ///< NOT owned by Codegen
    const TranslationUnitNode* ast; ///< the Abstract Syntax Tree
    SemaResult sema_result;         ///< array of semantic data (@c Type* and @c Symbol* pairs), keyed by their respective @c expression.id, populated by @c Sema in analysis time
    Emitter* emitter;               ///< the instruction-spelling engine.
    Text* assembly;
} Codegen;

Codegen* codegen_create(Context* context, const TranslationUnitNode* ast, SemaResult sema_result);
void codegen_destroy(Codegen* this);

Text* codegen_generate(Codegen* this);

#endif //C_COMPILER_CODEGEN_H
