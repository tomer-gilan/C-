//
// Created by tomer on 22-Sep-26.
//

#include "Lexer.h"
#include "Token.h"
#include "Utils.h"

#define LEXER_MAX_DECODE_LITERAL_LENGTH 4095
#define LEXER_EOF ((int)(-1))
#define LEXER_UNKNOWN_ESCAPE_CHARACTER ((int)(-2))

// ---------------------- Lexer ---------------------- //
struct Lexer {
    FilePosition cursor; ///< where the lexing currently stands.
    Context* context;
    Token token_eof; ///< token representing the end of lexing.
};

// C-tor, D-tor:
Lexer* lexer_create(Context* const context, const int source_file_index) {
    assert(context != NULL && "context is null");

    Lexer* lexer_stream = utils_xmalloc(sizeof(Lexer));
    *lexer_stream = (Lexer){
        .cursor = {.source_file_index = source_file_index, .offset = 0},
        .context = context,
        .token_eof = {
            .kind = TOKEN_EOF,
            .range = {
                .position = {
                    .source_file_index = source_file_index,
                    .offset = context_source_file_at(context, source_file_index)->length
                },
                .length = 0
            }
        }
    };
    return lexer_stream;
}
void lexer_destroy(Lexer* this) {
    free(this);
}

// Peeking, Popping:
/**
 * Peeks at a character, relative to the @c cursor.
 * @param this a handle to the lexer stream
 * @param offset the relative distance from the @c cursor
 * @return the character as an @c int
 */
static int lexer_peek_at(const Lexer* this, const int offset) {
    const SourceFile* const source = file_position_get_source_file(this->cursor, this->context);
    const int final_offset = this->cursor.offset + offset;
    return source_file_is_index_in_bounds(source, final_offset) ? (unsigned char)(source->text[final_offset]) : LEXER_EOF;
}
/**
 * Peeks the character the @c cursor points at.
 * @param this a handle to the lexer stream
 * @return the character as an @c int.
 * @retval LEXER_EOF if the relative @b offset is positioned out-of-bounds.
 */
static int lexer_peek(const Lexer* this) {
    return lexer_peek_at(this, 0);
}
/**
 * Pops the current character the @c cursor points at.
 * @param this a handle to the lexer stream
 * @return the character as an @c int
 */
static int lexer_pop(Lexer* this) {
    const int peek = lexer_peek(this);
    if (peek != LEXER_EOF)
        this->cursor.offset++;
    return peek;
}
/**
 * Pops characters until @b predicate is @c false.
 * @param this a handle to the lexer stream
 * @param predicate @c cursor of @c Lexer advances as long as @b predicate is true
 * @return the skipped @c Range
 */
static Range lexer_skip_while(Lexer* const this, bool (*predicate)(int character)) {
    const FilePosition start = this->cursor;
    while (predicate(lexer_peek(this)))
        lexer_pop(this);
    return range_from_file_positions(start, this->cursor);
}

// Predicates
/**
 * @param c character as an @c int
 * @retval true if @b character is a letter
 * @retval false otherwise
 */
static bool is_char_alpha(const int c) {
    return  ('A' <= c && c <= 'Z') ||
            ('a' <= c && c <= 'z');
}
/**
 * @param c character as an @c int
 * @retval true if @b character is a digit
 * @retval false otherwise
 */
static bool is_char_numeric(const int c) {
    return '0' <= c && c <= '9';
}
/**
 * @param c character as an @c int
 * @retval true if @b character is a word character
 * @retval false otherwise
 */
static bool is_char_word_character(const int c) {
    return  is_char_numeric(c) ||
            is_char_alpha(c)   ||
            c == '_';
}
/**
 * @param c character as an @c int
 * @retval true if @b character is a part of a punctuator's spelling.
 * @retval false otherwise
 */
static bool is_char_part_of_punctuator(const int c) {
    for (TokenKind kind = TOKEN__PUNC_FIRST; kind <= TOKEN__PUNC_LAST; ++kind) {
        const StringView spelling = token_kind_spelling_table[kind];
        for (int i = 0; i < spelling.length; ++i)
            if (spelling.data[i] == c)
                return true;
    }
    return false;
}
/**
 * @param c character as an @c int
 * @retval true if @b character is considered whitespace
 * @retval false otherwise
 */
static bool is_char_whitespace(const int c) {
    return c == '\t' || c == ' ' || c == '\n';
}

// Converters
/**
 * Converts @c StringView @b view to its integer form.
 * @attention asserts @c view contains numerics only. Does NOT check for a leading 0.
 */
static int lexer_string_view_to_int(const StringView view) {
    int number = 0;
    for (int i = 0; i < view.length; ++i) {
        const int c = (unsigned char)view.data[i];
        assert(is_char_numeric(c) && "non-numeric char");
        const int digit = c - '0';
        if (number > (INT_MAX - digit) / 10) { // checks for overflow before it happens, so no UB by definition.
            // TODO: issue diagnostic, too long numerical sequence
            return 0;
        }
        number *= 10;
        number += digit;
    }
    return number;
}
static int lexer_escape_character(const int character) {
    switch (character) {
        case '0':
            return 0x00;
        case 'a':
            return 0x07;
        case 'b':
            return 0x08;
        case 'f':
            return 0x0C;
        case 'n':
            return 0x0A;
        case 'r':
            return 0x0D;
        case 't':
            return 0x09;
        case 'v':
            return 0x0B;
        case '\\':
            return 0x5C;
        case '\'':
            return 0x27;
        case '\"':
            return 0x22;
        case '?':
            return 0x3F;
        default:
            return LEXER_UNKNOWN_ESCAPE_CHARACTER;
    }
}

// Lexing
/*
 * general note about the lexing functions.
 * The function checks for many invalid spellings and character sequences.
 * The general directive is:
 * 1. upon entering a lexing function, it asserts conditions on the first character. Since the callee should
 * (for example) call a character literal lexing if and only if a sequence beginning with '\'', it asserts this condition.
 * 2. any subsequent invalid states (like 123abc456 in an integer constant lexing) will be issued a diagnostic but will
 * not abort the process.
 * 3. other sanity checks to validate internal states and assure data structures not being corrupted etc. still asserts
 * conditions that may result in abortion of the process, but those should always be the result of a compiler bug, not
 * source file having some invalid text, since the compiler should collect diagnostic and report them to the user,
 * not crash upon discovery.
 */

/**
 * @internal to be used only by other lexing functions.
 * @param this a handle to the @c Lexer
 * @param buff4096 callee allocated buffer of 4096 bytes.
 * @param bytes_written the number of bytes written to @b buffer4096
 * @param delimiter ' or "
 * @return the @c Range of the literal
 */
static Range lexer_lex_literal_to_buffer(Lexer* const this, char* buff4096, int* const bytes_written, const char delimiter) {
    // otherwise it's invalid by definition:
    assert(buff4096 != NULL && "invalid buffer4096");
    assert((delimiter == '\'' || delimiter == '\"') && "Invalid delimiter");
    assert(lexer_peek(this) == delimiter && "First character of a char literal was not a delimiter.");

    const FilePosition start = this->cursor;
    lexer_pop(this); // pops the delimiter.

    bool should_escape_character = false;
    *bytes_written = 0;
    while (lexer_peek(this) != LEXER_EOF) {
        int c = lexer_pop(this);

        if (should_escape_character) {
            c = lexer_escape_character(c);
            if (c == LEXER_UNKNOWN_ESCAPE_CHARACTER) {
                /* TODO: issue diagnostic, invalid escape sequence, then CONTINUE MUNCHING!!! notice next we will
                 * store the UNKNOWN_CHARACTER as the character munched, and that's ok since we only need to get the
                 * literal spelling to continue lexing the source file. Since we issued a diagnostic, the compilation
                 * pipline will not finish by definition, so there's no risk by introducing garbage instead of the
                 * escaped character.
                 */
            }
            if (*bytes_written < LEXER_MAX_DECODE_LITERAL_LENGTH)
                buff4096[(*bytes_written)++] = (char)c;
            should_escape_character = false;
        }
        else if (c == '\\') { // i.e. if we encounter a non-escaped backslash, the next character SHOULD be escaped.
            should_escape_character = true;
        }
        else if (c == '\n') {
            // TODO: issue a diagnostic, missing delimiter.
            return range_from_file_positions(start, this->cursor);
        }
        else if (c == delimiter) { // i.e. if we encounter a non-escaped delimiter
            return range_from_file_positions(start, this->cursor);
        }
        else if (*bytes_written < LEXER_MAX_DECODE_LITERAL_LENGTH) // finally, the trivial case of a regular non escaped character.
                buff4096[(*bytes_written)++] = (char)c;
    }
    // if we got here something went wrong.
    // sanity check:
    assert(lexer_peek(this) == LEXER_EOF && "expected EOF");

    // TODO: issue diagnostic, unexpected EOF
    return range_from_file_positions(start, this->cursor);
}

/**
 * @note This function might have side effects such as updating the @c string_literal_table of @c Context,
 * and advancing the @c cursor of @c Lexer
 * @param this a handle to the @c Lexer
 * @retval TOKEN_KW_* if the lexeme matches a specific keyword
 * @retval TOKEN_IDENT otherwise.
 */
static Token lexer_lex_word(Lexer* const this) { // TOKEN_KW and TOKEN_IDENT
    assert(is_char_word_character(lexer_peek(this))); // otherwise it's invalid by definition.

    const Range range = lexer_skip_while(this, is_char_word_character);
    const StringView spelling = range_to_string_view(range, this->context);
    TokenKind kind = token_kind_from_spelling(spelling);

    assert(!token_kind_is_punctuator(kind) && "Somehow got a word spelling which is a punctuator.");

    if (token_kind_is_keyword(kind))
        return (Token){.kind = kind, .range = range};
    // Then it is an identifier.
    kind = TOKEN_IDENT;
    const int id = list_find_or_push(
        this->context->identifiers_table,
        &spelling, string_view_list_equals,
        string_view_list_duplicate);
    return (Token){.kind = kind, .range = range, .as.id = id};
}
static Token lexer_lex_punctuator(Lexer* const this) {
    assert(is_char_part_of_punctuator(lexer_peek(this))); // otherwise it's invalid by definition.

    Range range = range_from_file_positions(this->cursor, this->cursor);
    /* Notice the different munching strategy here. In word munching we naively munched every word character.
     * This case is more complicated, because we need to greedily munch the punctuator sequence, without swallowing
     * a punctuator that causes the overall sequence to be invalid (for example, ...+++... should be split ...++|+...)
     * and not ...+++|..., and you cannot pop a character and check it against the current range, because you cannot
     * unpop it.
     */
    while (is_char_part_of_punctuator(lexer_peek(this))) {
        range_extend(&range, 1);  // to include the current peeked punctuator (without actually popping it yet,
                                    // since the overall punctuator sequence may not be valid
        if (!token_kind_is_punctuator(token_kind_from_spelling(range_to_string_view(range, this->context)))) {
            range_extend(&range, -1); // excluding the last invalid punctuator.
            break; // invalid sequence, so we keep the maximal-munched sequence without the new punctuator.
        }
        lexer_pop(this); // the overall sequence WAS valid, so we maximal-munch this character as well.
    }
    return (Token){.kind = token_kind_from_spelling(range_to_string_view(range, this->context)), .range = range};
}
static Token lexer_lex_constant_int(Lexer* const this) {
    assert(is_char_numeric(lexer_peek(this))); // otherwise it's invalid by definition.

    const FilePosition start = this->cursor;
    bool calculate_value = true;
    if (lexer_peek(this) == '0' && is_char_numeric(lexer_peek_at(this, 1)))
        calculate_value = false; // for example, the token 01234 is invalid since its octal base which is unsupported.
    while (is_char_numeric(lexer_peek(this)))
        lexer_pop(this);
    if (is_char_word_character(lexer_peek(this))) {
        Range error_spelling = lexer_skip_while(this, is_char_word_character);
        calculate_value = false;
        // TODO: issue diagnostic, invalid numerical sequence
    }

    const Range range = range_from_file_positions(start, this->cursor);
    return (Token){
        .kind = TOKEN_CONST_INT,
        .range = range,
        .as.value = calculate_value ? lexer_string_view_to_int(range_to_string_view(range, this->context)) : 0
    };
}
static Token lexer_lex_constant_char(Lexer* const this) {
    // NOTE - yes, we return a TOKEN_CONST_INT, because the C standard states that char literals
    // should be interpreted as integer constants.

    char* buffer4096 = utils_xmalloc(sizeof(char) * (LEXER_MAX_DECODE_LITERAL_LENGTH + 1));
    int bytes_written;
    const Range range = lexer_lex_literal_to_buffer(this, buffer4096, &bytes_written, '\'');
    char value;
    if (bytes_written == 0) {
        value = 0;
        // TODO: issue diagnostic, "expected expression", set value of token to 0 and return. do not forget to free the buffer!!! or better yet, use the code already written and just have the value be the first character... probably better.
    }
    else {
        value = buffer4096[0];
        if (bytes_written > 1) {
            // TODO: issue diagnostic, multi-character literal, set value of token to 0 and return. do not forget to free the buffer!!! or better yet, use the code already written and just have the value be the first character... probably better.
        }
    }
    free(buffer4096);
    return (Token){
        .kind = TOKEN_CONST_INT,
        .range = range,
        .as.value = value
    };
}
static Token lexer_lex_string_literal(Lexer* const this) {
    char* buffer4096 = utils_xmalloc(sizeof(char) * (LEXER_MAX_DECODE_LITERAL_LENGTH + 1));
    int bytes_written;
    const Range range = lexer_lex_literal_to_buffer(this, buffer4096, &bytes_written, '\"');
    buffer4096[bytes_written] = '\0';
    const Token to_return = {
        .kind = TOKEN_STRING_LITERAL,
        .range = range,
        .as.id = list_find_or_push(
            this->context->string_literals_table,
            buffer4096,
            utils_string_list_strcmp,
            utils_string_list_allocate_and_copy)
    };
    free(buffer4096);
    return to_return;
}

static Token lexer_get_unknown_token(Lexer* const this) {
    const FilePosition start = this->cursor;
    lexer_pop(this);
    return (Token){
        .kind = TOKEN_UNKNOWN,
        .range = range_from_file_positions(start, this->cursor)
    };
}

/**
 * extract the next token based on the lexeme.
 * @param this a handle to the lexer stream
 * @return the next token based on the lexeme.
 */
static Token lexer_lex_token(Lexer* const this) {
    int c;
    while (is_char_whitespace(c = lexer_peek(this)))
        lexer_pop(this);
    if (c == LEXER_EOF)
        return this->token_eof;
    if (c == '\'')
        return lexer_lex_constant_char(this);
    if (c == '\"')
        return lexer_lex_string_literal(this);
    if (is_char_part_of_punctuator(c))
        return lexer_lex_punctuator(this);
    if (is_char_numeric(c))
        return lexer_lex_constant_int(this);
    if (c == '_' || is_char_alpha(c))
        return lexer_lex_word(this);
    // TODO: add comment lexing as another if (c == '/' or whatever)...

    // else: did not match any pattern, so c is unknown.
    // TODO: issue diagnostic, unknown character.
    return lexer_get_unknown_token(this);
}
List* lexer_lex_source_file(Lexer* const this) {
    List* token_list = list_create(free);
    Token* new_token;
    do {
        new_token = utils_xmalloc(sizeof(Token));
        *new_token = lexer_lex_token(this);
        list_push(token_list, new_token);
    } while (!token_is_EOF(*new_token));
    return token_list;
}