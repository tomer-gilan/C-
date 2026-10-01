//
// Created by tomer on 22-Sep-26.
//

#ifndef C_COMPILER_LEXER_H
#define C_COMPILER_LEXER_H

#include "Lexer.h"
#include "Context.h"
#include "List.h"

// ----------------- Lexer ----------------- //
typedef struct Lexer Lexer; // opaque struct

// C-tor and D-tor:
/**
 * @param context the @c compilation @c Context
 * @param source_file_index @c Context 's @c source_file_table index of the file to be lexed.
 * @return @c Lexer handle
 */
Lexer* lexer_create(Context* context, int source_file_index);
/**
 * @param this a handle to the @c Lexer
 */
void lexer_destroy(Lexer* this);

/**
 * Retrieves a token list based on the lexeme.
 * @warning ownership of returned @c List is transferred to the callee.
 * @param this a handle to the @c Lexer
 * @return a token list based on the lexeme.
 */
List* lexer_lex_source_file(Lexer* this);

#endif //C_COMPILER_LEXER_H