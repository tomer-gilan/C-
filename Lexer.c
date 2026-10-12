//
// Created by tomer on 22-Sep-26.
//

#include "Lexer.h"
#include "Token.h"
#include "Utils.h"
#include "Diagnostic.h"

#include <stdlib.h>
#include <assert.h>
#include <limits.h>


#define LEXER_EOF ((int)(-1))
#define LEXER_UNKNOWN_ESCAPE_CHARACTER ((int)(-2))

// ---------------------- Lexer ---------------------- //
struct Lexer {
    FilePosition cursor; ///< where the lexing currently stands.
    Context* context;
    Token token_eof; ///< token representing the end of lexing.
};

// C-tor, D-tor:
Lexer* lexer_create(Context* const context, const int source_text_index) {
    assert(context != NULL && "context is null");

    Lexer* lexer = utils_xmalloc(sizeof(Lexer));
    *lexer = (Lexer){
        .cursor = {.source_text_index = source_text_index, .offset = 0},
        .context = context,
        .token_eof = {
            .kind = TOKEN_EOF,
            .range = {
                .position = {
                    .source_text_index = source_text_index,
                    .offset = context_source_text_at(context, source_text_index)->data->length
                },
                .length = 0
            }
        }
    };
    return lexer;
}
void lexer_destroy(Lexer* this) {
    free(this);
}

// Peeking, Popping:
/**
 * Peeks at a character, relative to the @c cursor.
 * @param this a handle to the lexer
 * @param offset the relative distance from the @c cursor
 * @return the character as an @c int
 * @retval LEXER_EOF if the relative @b offset is positioned out-of-bounds.
 */
static int lexer_peek_at(const Lexer* this, const int offset) {
    const SourceText* const source = context_source_text_at(this->context, this->cursor.source_text_index);
    const int final_offset = this->cursor.offset + offset;
    return source_text_is_index_in_bounds(source, final_offset) ? (unsigned char)(source->data->data[final_offset]) : LEXER_EOF;
}
/**
 * Peeks the character the @c cursor points at.
 * @param this a handle to the lexer
 * @return the character as an @c int.
 * @retval LEXER_EOF if no more tokens are left.
 */
static int lexer_peek(const Lexer* this) {
    return lexer_peek_at(this, 0);
}
/**
 * Pops the current character the @c cursor points at.
 * @param this a handle to the lexer
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
 * @param this a handle to the lexer
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
 * @param c a character as an @c int
 * @retval true if @b character is a letter
 * @retval false otherwise
 */
static bool is_char_alpha(const int c) {
    return  ('A' <= c && c <= 'Z') ||
            ('a' <= c && c <= 'z');
}
/**
 * @param c a character as an @c int
 * @retval true if @b character is a digit
 * @retval false otherwise
 */
static bool is_char_numeric(const int c) {
    return '0' <= c && c <= '9';
}
/**
 * @param c a character as an @c int
 * @retval true if @b character is a word character
 * @retval false otherwise
 */
static bool is_char_word_character(const int c) {
    return  is_char_numeric(c) ||
            is_char_alpha(c)   ||
            c == '_';
}
/**
 * @param c a character as an @c int
 * @retval true if @b character is a part of a punctuator's spelling.
 * @retval false otherwise
 */
static bool is_char_part_of_punctuator(const int c) {
    for (TokenKind kind = TOKEN__PUNC_FIRST; kind <= TOKEN__PUNC_LAST; ++kind) {
        const TextView spelling = token_kind_spelling_table[kind];
        for (int i = 0; i < spelling.length; ++i)
            if (spelling.data[i] == c)
                return true;
    }
    return false;
}
/**
 * @param c a character as an @c int
 * @retval true if @b character is considered whitespace
 * @retval false otherwise
 */
static bool is_char_whitespace(const int c) {
    return  c == ' '  ||
            c == '\t' ||
            c == '\n' ||
            c == '\v' ||
            c == '\f' ||
            c == '\r';
}

// Converters
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
 * 1. upon entering a lexing function, it asserts conditions on the first character. Since the caller should
 * (for example) call a character literal lexing if and only if a sequence beginning with '\'', it asserts this condition.
 * 2. any subsequent invalid states (like 123abc456 in an integer constant lexing) will be issued a diagnostic but will
 * not abort the process.
 * 3. other sanity checks to validate internal states and assure data structures not being corrupted etc. still asserts
 * conditions that may result in abortion of the process, but those should always be the result of a compiler bug, not
 * source text having some invalid text, since the compiler should collect diagnostic and report them to the user,
 * not crash upon discovery.
 */

/**
 * @internal to be used only by other lexing functions.
 * @warning ownership of return value is transferred to the caller.
 * @param this a handle to the @c Lexer
 * @param delimiter ' or "
 * @param reported flag indicating weather a diagnostic has been reported during lexing.
 * @return the @c Text of the literal
 */
static Text *lexer_lex_literal_to_text(Lexer* const this, const char delimiter, bool* const reported) {
    *reported = false;

    // if one of these is false, it's invalid by definition:
    assert((delimiter == '\'' || delimiter == '\"') && "Invalid delimiter");
    assert(lexer_peek(this) == delimiter && "First character of a char literal was not a delimiter.");

    const FilePosition start = this->cursor;
    lexer_pop(this); // pops the delimiter.

    Text* literal_text = text_create_empty();
    bool should_escape_character = false;
    while (lexer_peek(this) != LEXER_EOF) {
        int c = lexer_pop(this);

        if (c == '\n') {
            context_report_diagnostic(
                this->context,
                delimiter == '"' ? DIAGNOSTIC_CODE_LEXER_EXPECTED_QUOTATION_MARKS : DIAGNOSTIC_CODE_LEXER_EXPECTED_APOSTROPHE,
                range_from_file_positions(start, this->cursor));
            *reported = true;
            return literal_text;
        }

        if (should_escape_character) {
            c = lexer_escape_character(c);
            if (c == LEXER_UNKNOWN_ESCAPE_CHARACTER) {
                FilePosition invalid_sequence_start = this->cursor;
                invalid_sequence_start.offset -= 2;
                /* issue diagnostic, invalid escape sequence, then CONTINUE MUNCHING!!! notice next we will
                 * store the UNKNOWN_CHARACTER as the character munched, and that's ok since we only need to get the
                 * literal spelling to continue lexing the source text. Since we issued a diagnostic, the compilation
                 * pipline will not finish by definition, so there's no risk by introducing garbage instead of the
                 * escaped character.
                 */
                context_report_diagnostic(
                    this->context,
                    DIAGNOSTIC_CODE_LEXER_INVALID_ESCAPE_SEQUENCE,
                    range_from_file_positions(invalid_sequence_start, this->cursor));
                *reported = true;
            }
            text_push(literal_text, (char)c);
            should_escape_character = false;
        }
        else if (c == '\\') { // i.e. if we encounter a non-escaped backslash, the next character SHOULD be escaped.
            should_escape_character = true;
        }
        else if (c == delimiter) { // i.e. if we encounter a non-escaped delimiter
            return literal_text;
        }
        else
            text_push(literal_text, (char)c);
    }
    // if we got here something went wrong.
    // sanity check:
    assert(lexer_peek(this) == LEXER_EOF && "expected EOF");

    context_report_diagnostic(
        this->context,
        DIAGNOSTIC_CODE_LEXER_UNEXPECTED_EOF,
        range_from_file_positions(start, this->cursor));
    *reported = true;
    return literal_text;
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
    const TextView spelling = context_range_to_text_view(this->context, range);
    const TokenKind kind = token_kind_from_spelling(spelling);

    assert(!token_kind_is_punctuator(kind) && "Somehow got a word spelling which is a punctuator.");

    if (token_kind_is_keyword(kind))
        return (Token){.kind = kind, .range = range};
    // Then it is an identifier.
    return (Token){.kind = TOKEN_IDENT, .range = range, .as.id = context_intern_identifier(this->context, spelling)};
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
        if (!token_kind_is_punctuator(token_kind_from_spelling(context_range_to_text_view(this->context, range)))) {
            range_extend(&range, -1); // excluding the last invalid punctuator.
            break; // invalid sequence, so we keep the maximal-munched sequence without the new punctuator.
        }
        lexer_pop(this); // the overall sequence WAS valid, so we maximal-munch this character as well.
    }
    return (Token){
        .kind = token_kind_from_spelling(context_range_to_text_view(this->context, range)),
        .range = range
    };
}
static Token lexer_lex_constant_int(Lexer* const this) {
    assert(is_char_numeric(lexer_peek(this))); // otherwise it's invalid by definition.

    bool should_calculate_value = true;
    const FilePosition start = this->cursor; // is used both for the token's range and the value parsing diagnostic.
    if (lexer_peek(this) == '0' && is_char_numeric(lexer_peek_at(this, 1))) {
        should_calculate_value = false;
        // for example, the token 01234 is invalid since its octal base which is unsupported.
        context_report_diagnostic(
            this->context,
            DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_PREFIX,
            (Range){.position = start, .length = 1});
    }
    while (is_char_numeric(lexer_peek(this)))
        lexer_pop(this);
    if (should_calculate_value && is_char_word_character(lexer_peek(this))) {
        should_calculate_value = false;
        context_report_diagnostic(
            this->context,
            DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE,
            lexer_skip_while(this, is_char_word_character));
    }
    const Range range = range_from_file_positions(start, this->cursor);

    // integer parsing:
    int value = 0;
    if (should_calculate_value) {
        const TextView spelling = context_range_to_text_view(this->context, range);
        for (int i = 0; i < range.length; ++i) {
            const int c = (unsigned char) spelling.data[i];
            assert(is_char_numeric(c) && "non-numeric char");
            const int digit = c - '0';
            if (value > (INT_MAX - digit) / 10) {
                // checks for overflow before it happens, so no UB by definition.
                context_report_diagnostic(
                    this->context,
                    DIAGNOSTIC_CODE_LEXER_NUMERIC_CONSTANT_OUT_OF_BOUNDS,
                    range);
                value = 0;
                break;
            }
            value *= 10;
            value += digit;
        }
    }

    return (Token){
        .kind = TOKEN_CONST_INT,
        .range = range,
        .as.value = value
    };
}
static Token lexer_lex_constant_char(Lexer* const this) {
    // NOTE - yes, we return a TOKEN_CONST_INT, because the C standard states that char literals
    // should be interpreted as integer constants.
    bool reported = false;
    const FilePosition start = this->cursor;
    Text* const text = lexer_lex_literal_to_text(this, '\'', &reported);
    const Range range = range_from_file_positions(start, this->cursor);

    char value;
    if (text_is_empty(text)) {
        value = 0;
        if (!reported) {
            context_report_diagnostic(
                this->context,
                DIAGNOSTIC_CODE_LEXER_EXPECTED_EXPRESSION,
                range);
            reported = true;
        }
    }
    else {
        value = text->data[0]; // data is not empty, so a first value is guaranteed.
        if (text->length > 1) {
            context_report_diagnostic(
                this->context,
                DIAGNOSTIC_CODE_LEXER_MULTI_CHARACTER_LITERAL,
                range);
        }
    }

    text_destroy(text);
    return (Token){
        .kind = TOKEN_CONST_INT,
        .range = range,
        .as.value = value
    };
}
static Token lexer_lex_string_literal(Lexer* const this) {
    bool reported = false;
    const FilePosition start = this->cursor;
    Text* const text = lexer_lex_literal_to_text(this, '\"', &reported);
    const Range range = range_from_file_positions(start, this->cursor);

    const Token to_return = {
        .kind = TOKEN_STRING_LITERAL,
        .range = range,
        .as.id = context_intern_string_literal(this->context, text)
    };
    text_destroy(text);
    return to_return;
}
/**
 * an unknown token represents a single unknown character, encountered when parsing.
 */
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
 * @param this a handle to the lexer
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
    const Token unknown_token = lexer_get_unknown_token(this);
    context_report_diagnostic(
        this->context,
        DIAGNOSTIC_CODE_LEXER_UNKNOWN_CHARACTER,
        unknown_token.range);
    return unknown_token;
}
List* lexer_lex_source_text(Lexer* const this) {
    List* token_list = list_create(free);
    Token* new_token;
    do {
        new_token = utils_xmalloc(sizeof(Token));
        *new_token = lexer_lex_token(this);
        list_push(token_list, new_token);
    } while (!token_is_eof(*new_token));
    return token_list;
}