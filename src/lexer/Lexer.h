//
// Created by tomer on 22-Sep-26.
//

#ifndef C_COMPILER_LEXER_H
#define C_COMPILER_LEXER_H

#include "Context.h"
#include "../data_structures/List.h"

// ----------------- Lexer ----------------- //
typedef struct Lexer Lexer; // opaque struct

// C-tor and D-tor:
/**
 * @attention ownership of @b context remains with the caller, i.e. @b context must outlive @c Lexer.
 * @param context the @c compilation @c Context
 * @param source_text_index @c Context 's @c source_text_table index of the file to be lexed.
 * @return @c Lexer handle
 */
Lexer* lexer_create(Context* context, int source_text_index);
/**
 * @param this a handle to the @c Lexer
 */
void lexer_destroy(Lexer* this);

/**
 * Retrieves a token list based on the lexeme.
 * @warning ownership of returned @c List is transferred to the caller.
 * @param this a handle to the @c Lexer
 * @return a token list based on the lexeme.
 */
List* lexer_lex_source_text(Lexer* this);

#endif //C_COMPILER_LEXER_H