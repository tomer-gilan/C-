//
// Created by tomer on 22-Sep-26.
//

#ifndef GIT_CODE_LEXER_H
#define GIT_CODE_LEXER_H

#include <stdlib.h>
#include <errno.h>

#include "Diagnostic.h"
#include "Token.h"
#include "Globals.h"

typedef struct MunchResult MunchResult;

typedef struct LexerStream LexerStream; // opaque pointer, deliberately chosen so.

// ----------------- static variables ----------------- //
static const char* const keywords_table[TOKEN_KEYWORD_ID__COUNT] = {
    "char",
    "else",
    "if",
    "int",
    "return",
    "void",
    "while"
};
static const char* const punctuators_table[TOKEN_PUNCTUATOR_ID__COUNT] = {
    "[", "]", "(", ")",
    "{", "}", "&", "*",
    "+", "-", "~", "!",
    "/", "%", "^", "=",
    "<<", ">>", ">", "<",
    ">=", "<=", "==", "!=",
    "|", "&&", "||", ";"
};

// ----------------- helpers ----------------- //
/**
 *
 * @param string string
 * @return the keyword id that the string represents. -1 if no not found.
 */
static TokenKeywordID lexer_get_keyword_id_by_string(const char* const string) {
    return utils_string_array_index_of(string, keywords_table, TOKEN_KEYWORD_ID__COUNT);
}
/**
 *
 * @param string string
 * @return the punctuator id that the string represents. -1 if no not found.
 */
static TokenPunctuatorID lexer_get_punctuator_id_by_string(const char* const string) {
    return utils_string_array_index_of(string, punctuators_table, TOKEN_PUNCTUATOR_ID__COUNT);
}

// ----------------- LexerStream ----------------- //

// C-tor and D-tor:
/**
 * Create a new LexerStream struct.
 * @param data the lexeme that the lexer should parse
 * @param size the size of data
 * @return a LexerStream handle
 */
LexerStream* lexer_stream_create(const char* data, int size);

/**
 * Destroys the LexerStream handle
 * @param this a handle to the lexer stream
 */
void lexer_stream_destroy(LexerStream* this);

// Peeking, Popping:
/**
 * Peeks the current char of the lexeme.
 * @param this a handle to the lexer stream
 * @return the current char of the lexeme. LEXER_EOS if EOS was reached.
 */
byte lexer_stream_peek(const LexerStream* this);

/**
 * Retrieves the current char of the lexeme, then advances SPI.
 * @param this a handle to the lexer stream
 * @return the current char of the lexeme.
 */
byte lexer_stream_pop(LexerStream* this);

/**
 * Peeks a character, relative to SPI.
 * @param this a handle to the lexer stream
 * @param offset the relative distance between SPI and the peeked character.
 * @return the peeked character relative to SPI. LEXER_EOS if EOS was reached.
 */
byte lexer_stream_peek_at(const LexerStream* this, unsigned int offset);

/**
 * Skips "offset" bytes from SPI
 * @attention if fewer bytes than offset remain until EOS, SPI will be capped and set to the last valid position.
 * @param this a handle to the lexer stream
 * @param offset the relative distance between SPI and the next position.
 */
void lexer_stream_skip(LexerStream* this, unsigned int offset);

/**
 * Peeks the current ESCAPED char of the lexeme.
 * @attention assumes we are inside a literal (i.e. within the ("), (') walls).
 * Also, it may SYNTHETICALLY escape the char, depending on the character.
 * (more on that in lexer_is_synthetically_escaped)
 * @param this a handle to the lexer stream
 * @param delimiter (") or ('), according to which type of literal we're peeking from.
 * @return the current ESCAPED char of the lexeme.
 */
byte lexer_stream_peek_escaped(const LexerStream* this, byte delimiter);

/**
 * Retrieves the current ESCAPED char of the lexeme, then advances SPI.
 * @attention assumes we are inside a literal (i.e. within the ("), (') walls).
 * Also, it may SYNTHETICALLY escape the char, depending on the character.
 * (more on that in lexer_is_synthetically_escaped)
 * @param this a handle to the lexer stream
 * @param delimiter (") or ('), according to which type of literal we're peeking from.
 * @return the current ESCAPED char of the lexeme.
 */
byte lexer_stream_pop_escaped(LexerStream* this, byte delimiter);

// Tokenizing:
/**
 * extract the next token based on the lexeme.
 * @param this a handle to the lexer stream
 * @return the next token based on the lexeme.
 */
Token lexer_stream_extract_token(LexerStream* this);

/**
 * Retrieves a token list based on the lexeme.
 * @param this a handle to the list
 * @return a token list based on the lexeme.
 */
List* lexer_stream_extract_all_tokens(LexerStream* this);

#endif //GIT_CODE_LEXER_H