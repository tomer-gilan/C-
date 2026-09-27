//
// Created by tomer on 22-Sep-26.
//

#include "Lexer.h"

#include <inttypes.h>
#include <limits.h>

#define LEXER_MIN_STRING_LITERAL_DIFFERENTIATION_LENGTH 4096
#define LEXER_MIN_IDENTIFIER_DIFFERENTIATING_LENGTH 64
#define LEXER_MAX_PUNCTUATOR_LENGTH 8
#define LEXER_MAX_NUMERIC_CONSTANT_LENGTH 128
// ^^^ actually its ~64 (0b0000...00000, in x64 architecture) but I gave it some leeway.
#define LEXER_EOS 0xFF

// --------- predicates --------- //
/**
 *
 * @param string string
 * @return true iff 'string' is a keyword
 */
static bool lexer_is_keyword(const char* const string) {
    return lexer_get_keyword_id_by_string(string) != -1;
}
/**
 *
 * @param string string
 * @return true iff 'string' is a punctuator
 */
static bool lexer_is_punctuator(const char* const string) {
    return lexer_get_punctuator_id_by_string(string) != -1;
}

/**
 *
 * @param c character
 * @return TRUE iff c is a letter
 */
static bool lexer_is_char_alpha(const byte c) {
    return  ('A' <= c && c <= 'Z') ||
            ('a' <= c && c <= 'z');
}
/**
 *
 * @param c character
 * @return true iff c is a digit.
 */
static bool lexer_is_char_numeric(const byte c) {
    return '0' <= c && c <= '9';
}
/**
 *
 * @param c character
 * @return true iff c is alphanumeric or '_' (underscore)
 */
static bool lexer_is_char_word_character(const byte c) {
    return lexer_is_char_numeric(c) || lexer_is_char_alpha(c) || c == '_';
}
/**
 *
 * @param c character
 * @return true iff c is a punctuator (or a part of one)
 */
static bool lexer_is_char_punctuator(const byte c) {
    for (int i = 0; i < TOKEN_PUNCTUATOR_ID__COUNT; ++i)
        for (int j = 0; punctuators_table[i][j] != '\0'; ++j)
            if (punctuators_table[i][j] == c)
                return true;
    return false;
}
/**
 *
 * @param c character
 * @return true iff c is EOS (0xFF)
 */
static bool lexer_char_is_EOS(const byte c) {
    return c == LEXER_EOS;
}
/**
 *
 * @param c character
 * @return true iff c is a string delimiter.
 */
static bool lexer_is_char_string_delimiter(const byte c) {
    return c == '\'' || c == '\"';
}
/**
 *
 * @param c character
 * @return true iff the lexer is indifferent to c.
 */
static bool lexer_is_char_parsing_indifferent(const byte c) {
    return c == '\t' || c == ' ' || c == '\n';
}

// ----------- classifiers --------------- //
/**
 * The rational and need behind this enum:
 * For descriptive error messages, if the middle of (for example) the token "123abc456" is invalid,
 * we still want to munch though all of it, to later print the whole token when issuing the diagnostic.
 * So, we need to classify three different states:
 * 1. the character is valid for the given token catagory.
 * 2. the character is invalid, but the munch should continue.
 * 3. the character is not a part of the sequence, and the munch should be terminated.
 * where in case #1 and #2 we continue munching, until a type #3 character is encountered.
 */
typedef enum {
    MUNCH_CLASSIFICATION_VALID,
    MUNCH_CLASSIFICATION_INVALID_CONTINUE,
    MUNCH_CLASSIFICATION_TERMINATE
} MunchClassification;

static MunchClassification classify_word(const byte c) {
    if (lexer_is_char_word_character(c))
        return MUNCH_CLASSIFICATION_VALID;
    return MUNCH_CLASSIFICATION_TERMINATE;
}
static MunchClassification classify_numeric(const byte c) {
    if (lexer_is_char_numeric(c))
        return MUNCH_CLASSIFICATION_VALID;
    if (c == '_' || lexer_is_char_alpha(c))
        return MUNCH_CLASSIFICATION_INVALID_CONTINUE;
    return MUNCH_CLASSIFICATION_TERMINATE;
}

// ----------- escaping ---------------- //
static bool lexer_is_structural_character(const byte c) {
    return (c & 0x80) == 0x80; // 0x80 == 0b1000:0000, so it essentially checks if MSB is 1.
}
/**
 * sets c's MSB to new_MSB
 * @param c character
 * @param new_MSB the new value to be set
 * @return the new character, after MSB set.
 */
static byte lexer_set_MSB(const byte c, const bool new_MSB) {
    return (c & 0x7F) | (new_MSB << 7);
}
static byte lexer_change_structural_status(const byte c) {
    return lexer_set_MSB(c, !lexer_is_structural_character(c));
}
static bool lexer_is_structural_version_of(const byte to_test, const byte test_against) {
    if (!lexer_is_structural_character(to_test))
        return false;
    return lexer_change_structural_status(to_test) == test_against;
}

// --------- converters ------------- //
/**
 *
 * @param c character
 * @return numeric representation of the character
 */
static int lexer_char_to_digit(const byte c) {
    return c - '0';
}
/**
 * Converts 'string' to it's integer form.
 * @attention throws error when string is invalid.
 * @param string the string to be converted
 * @param position the position of the original string (for diagnostic issuing)
 * @return the numeric equivalence of the string
 */
static int lexer_string_to_int(const char* string, const Position position) {
    char* end;
    errno = 0;
    const long number = strtol(string, &end, 0);

    if (*end != '\0') {
        diagnostic_issue_string(DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SUFFIX, position, end);
    }
    // Now we know the entire string was numerically valid.
    // Checking bounds:
    if (errno == ERANGE || number > INT_MAX) {
        diagnostic_issue_string(DIAGNOSTIC_CODE_LEXER_NUMERIC_SEQUENCE_OUT_OF_BOUNDS, position, end);
    }
    // Number is both numerically valid and in bounds:
    return (int)number;
}

// ---------------------- LexerStream ---------------------- //
struct MunchResult {
    int bytes_munched;
    bool valid_munch;
};
struct LexerStream{
    unsigned int SPI; // SPI: Stream Position Indicator. i.e., the index of the next char to parse.
    char* data;
    unsigned int size;
    Position position;
};

// Helpers:
/**
 *
 * @param this a handle to the lexer stream
 * @param index the index in question
 * @return true iff index is in bounds
 */
static bool lexer_stream_is_index_in_bounds(const LexerStream* this, const unsigned int index) {
    return index < this->size;
}

// C-tor, D-tor:
LexerStream* lexer_stream_create(const char* const data, const int size) {
    LexerStream* lexer_stream = malloc(sizeof(LexerStream));
    *lexer_stream = (LexerStream){
        .SPI = 0,
        .data = utils_allocate_and_copy_string(data),
        .size = size,
        .position = {.line = 0, .column = 0}
    };
    return lexer_stream;
}
void lexer_stream_destroy(LexerStream* this) {
    free(this->data);
    free(this);
}

// Peeking, Popping:
byte lexer_stream_peek_at(const LexerStream* this, const unsigned int offset) {
    const unsigned int final_index = this->SPI + offset;
    if (!lexer_stream_is_index_in_bounds(this, final_index))
        return LEXER_EOS;
    return this->data[final_index];
}
byte lexer_stream_peek(const LexerStream* this) {
    return lexer_stream_peek_at(this, 0);
}
byte lexer_stream_pop(LexerStream* this) {
    if (this->SPI == this->size)
        return LEXER_EOS;
    // Update position:
    if (lexer_stream_peek(this) == '\n')
        location_set_new_line(&this->position);
    else
        location_increment(&this->position);

    return this->data[this->SPI++];
}
void lexer_stream_skip(LexerStream* this, const unsigned int offset) {
    for (int i = 0; i < offset; ++i)
        lexer_stream_pop(this); // why? that's the easiest way to ensure all invariants hold, like line & column
                                // correctness. If you simply SPI += offset, you will need to loop through everything
                                // to know how much new lines were there, so you don't gain performance by doing that.
}

/**
 * @attention This is an internal function. Not to be used as part of the public API.
 * @param this a handle to the lexer stream
 * @param ptr a pointer to be filled with the peeked char.
 * @param delimiter (") or ('), according to which type of literal we're peeking from.
 * @return number of bytes read
 */
static int lexer_stream_peek_escaped_internal(const LexerStream* this, byte* ptr, const byte delimiter) {
    byte first = lexer_stream_peek(this);

    if (lexer_is_structural_character(first)) {
        diagnostic_issue(DIAGNOSTIC_CODE_LEXER_STRUCTURAL_CHARACTER_ENCOUNTERED, this->position);
        return 0;
    }
    if (first != '\\') {
        // Now we know that this character is not escaped.
        if (first == delimiter)
            first = lexer_change_structural_status(first); // makes the character structural
        // pack it up and finish
        *ptr = first;
        return 1;
    }

    // Now, we have an escape sequence to decode. Right now only 1-byte-wide sequences are supported.
    const byte second = lexer_stream_peek_at(this, 1);
    if (lexer_is_structural_character(second)) {
        diagnostic_issue(DIAGNOSTIC_CODE_LEXER_STRUCTURAL_CHARACTER_ENCOUNTERED, this->position);
        return 0;
    }
    switch (second) {
        case 'a':
            *ptr = 0x07;
            break;
        case 'b':
            *ptr = 0x08;
            break;
        case 't':
            *ptr = 0x09;
            break;
        case 'n':
            *ptr = 0x10;
            break;
        case 'v':
            *ptr = 0x11;
            break;
        case 'f':
            *ptr = 0x12;
            break;
        case 'r':
            *ptr = 0x13;
            break;
        case '\\':
        case '\'':
        case '\"':
            *ptr = second;
            break;
        case '0':
            *ptr = 0;
            break;
        default:
            diagnostic_issue_char(DIAGNOSTIC_CODE_LEXER_INVALID_ESCAPE_CHARACTER, this->position, second);
            break;
    }
    return 2; // two-bytes were read.
}
byte lexer_stream_peek_escaped(const LexerStream* this, const byte delimiter) {
    byte to_return;
    lexer_stream_peek_escaped_internal(this, &to_return, delimiter);
    return to_return;
}
byte lexer_stream_pop_escaped(LexerStream* this, const byte delimiter) {
    byte to_return;
    lexer_stream_skip(this, lexer_stream_peek_escaped_internal(this, &to_return, delimiter));
    return to_return;
}

// Maximal Munching:
/**
 * Maximal munch is a doctrine according to which, when matching an identifier, keyword, punctuator, etc.
 * we greedily try to match the longest sequence, according to some predicate.
 * Note: the C standard specifies different minimum differentiable lengths for different types of token.
 * For example, two identifiers of lengths more than 63 bytes can be differentiated by the first 63 bytes alone.
 * In practice, that means that while we munch through the entire identifier word, we save only the first 63 bytes to
 * the buffer.
 * @param this a handle to the @code LexerStream @endcode.
 * @param buff a caller allocated buffer of @code buff_size @endcode size, which is populated by the matched string.
 * @param buff_size buffer size (in bytes).
 * @param classifier classifies the characters (for more info, read the @code MunchClassification @endcode docs)
 * @return the number of bytes written into the buffer.
 */
static MunchResult lexer_stream_maximal_munch_text(LexerStream* const this, byte* const buff, const int buff_size, MunchClassification (*classifier)(byte c)) {
    int index = 0;
    MunchClassification mc;
    bool is_valid_munch = true;
    while ((mc = classifier(lexer_stream_peek(this))) != MUNCH_CLASSIFICATION_TERMINATE) {
        if (mc == MUNCH_CLASSIFICATION_INVALID_CONTINUE)
            is_valid_munch = false;
        const byte b = lexer_stream_pop(this);
        if (index < buff_size)
            buff[index++] = b;
    }
    buff[index] = '\0';
    return (MunchResult){.bytes_munched = index, .valid_munch = is_valid_munch};
}
/**
 * Extracts the maximal munch, starting at SPI, consisting of A-Z,a-z,'_'
 * (AKA "word"), and stores it to a caller allocated buffer.
 * @attention if the current text chain contains more than 64 characters, the rest will be popped
 * from the stream, and essentially discarded.
 * The C standard allows us to disregard the suffix of names that are longer than 64 characters.
 * @param this a handle to the lexer stream
 * @param buff64 a caller allocated 64 byte buffer where the text is extracted to.
 * @return the number of bytes written into the buffer.
 */
static MunchResult lexer_stream_maximal_munch_word(LexerStream* this, byte* buff64) {
    return lexer_stream_maximal_munch_text(
        this,
        buff64,
        LEXER_MIN_IDENTIFIER_DIFFERENTIATING_LENGTH,
        classify_word);
}
/**
 * Extracts the maximal munch, starting at SPI, consisting of punctuations
 * (as detailed in the punctuators_table), and stores it to a caller allocated buffer.
 * @param this a handle to the lexer stream
 * @param buff8 a caller allocated 8 byte buffer where the punctuation is extracted to.
 * @return the number of bytes written into the buffer.
 */
static MunchResult lexer_stream_maximal_munch_punctuator(LexerStream* this, byte* buff8) {
    int index = 0;
    buff8[0] = '\0';
    do {
        if (index == LEXER_MAX_PUNCTUATOR_LENGTH - 1) {
            diagnostic_issue_string(DIAGNOSTIC_CODE_LEXER_INVALID_PUNCTUATOR_SEQUENCE, this->position, buff8);
            return (MunchResult){.bytes_munched = index, .valid_munch = false};
        }
        buff8[index++] = lexer_stream_pop(this);
        buff8[index] = '\0';
    } while (lexer_is_punctuator(buff8));
    buff8[--index] = '\0';
    return (MunchResult){.bytes_munched = index, .valid_munch = true};
}
/**
 * Maximal munch is a doctrine according to which, when matching an identifier, keyword, punctuator, etc.
 * we greedily try to match the longest sequence, according to some predicate.
 * @attention the C standard specifies different minimum differentiable lengths for different types of token.
 * For literals (characters and string), the minimum differentiable length required is 4095 characters long.
 * In practice, that means that while we munch through the entire identifier word, we save only the first 63 bytes to
 * the buffer.
 * ALSO: the function assumes the munch begins AFTER the first delimiter was popped.
 * @param this a handle to the @code LexerStream @endcode.
 * @param buff4096 the buffer the literal is written into.
 * @return the number of bytes written into the buffer.
 */
static MunchResult lexer_stream_maximal_munch_string_literal(LexerStream* this, byte* buff4096) {
    const byte delimiter = '\"';
    int index = 0;
    bool is_valid_munch = true;
    byte current;
    do {
        current = lexer_stream_pop_escaped(this, delimiter);
        if (lexer_char_is_EOS(current)) {
            is_valid_munch = false;
            diagnostic_issue(DIAGNOSTIC_CODE_LEXER_UNEXPECTED_EOS, this->position);
            break;
        }
        if (index < LEXER_MIN_STRING_LITERAL_DIFFERENTIATION_LENGTH - 1)
            buff4096[index++] = current;
    } while (!lexer_is_structural_version_of(current, delimiter));
    buff4096[index] = '\0';
    return (MunchResult){.bytes_munched = index, .valid_munch = is_valid_munch};
}
/**
 * Maximal munch is a doctrine according to which, when matching an identifier, keyword, punctuator, etc.
 * we greedily try to match the longest sequence, according to some predicate.
 * @attention the C standard specifies different minimum differentiable lengths for different types of token.
 * For literals (characters and string), the minimum differentiable length required is 4095 characters long.
 * In practice, that means that while we munch through the entire identifier word, we save only the first 63 bytes to
 * the buffer.
 * ALSO: the function assumes the munch begins AFTER the first delimiter was popped.
 * @param this a handle to the @code LexerStream @endcode.
 * @param buff4096 the buffer the literal is written into.
 * @return the number of bytes written into the buffer.
 */
static MunchResult lexer_stream_maximal_munch_character_literal(LexerStream* this, byte* buff4096) {
    const byte delimiter = '\'';
    int index = 0;

    const byte first = lexer_stream_pop_escaped(this, delimiter);
    if (lexer_char_is_EOS(first)) {
        diagnostic_issue(DIAGNOSTIC_CODE_LEXER_UNEXPECTED_EOS, this->position);
        return (MunchResult){.bytes_munched = 0, .valid_munch = false}; // 0 bytes munched, because we already hit EOS.
    }
    if (lexer_is_structural_version_of(first, delimiter)) {
        diagnostic_issue(DIAGNOSTIC_CODE_LEXER_STRUCTURAL_CHARACTER_ENCOUNTERED, this->position);
        return (MunchResult){.bytes_munched = 0, .valid_munch = false};  // 0 bytes munched, because a delimiter does not count
                                                              // as a byte.
    }
    buff4096[index++] = first;
    const bool is_valid_munch =
        lexer_is_structural_version_of(lexer_stream_peek_escaped(this, delimiter), delimiter);

    // in case the character literal has more than 1 (escaped) characters, we munch through them, and flag
    // the sequence as invalid
    byte current;
    do {
        current = lexer_stream_pop_escaped(this, delimiter);
        if (lexer_char_is_EOS(current)) {
            diagnostic_issue(DIAGNOSTIC_CODE_LEXER_UNEXPECTED_EOS, this->position);
            break;
        }
        if (index < LEXER_MIN_STRING_LITERAL_DIFFERENTIATION_LENGTH - 1)
            buff4096[index++] = current;
    } while (!lexer_is_structural_version_of(current, delimiter));
    buff4096[index] = '\0';
    return (MunchResult){.bytes_munched = index, .valid_munch = is_valid_munch};
}
/**
 * Maximal munch is a doctrine according to which, when matching an identifier, keyword, punctuator, etc.
 * we greedily try to match the longest sequence, according to some predicate.
 * @attention For numeric (characters and string), there's no hard maximum valid sequence length specified in the C
 * standard, like for literal and identifiers for example.
 * However, but the numeric value should fit in the destination type (int, long, etc.).
 * As of right now, the compiler only supports the (signed) 'int' numeric type, so this is the current constraint.
 * Numeric sequences (aside from counting-base-prefixes) can only contain 0-9 characters (and (')).
 * In order to later issue a descriptive diagnostic, we maximal-munch through the entire (albeit invalid) token anyway.
 * @param this a handle to the @code LexerStream @endcode.
 * @param buff128 the buffer the numeric is written into.
 * @return the number of bytes written into the buffer.
 */
static MunchResult lexer_stream_maximal_munch_numeric(LexerStream* this, byte* buff128) {
    const char first = lexer_stream_peek(this);
    if (!lexer_is_char_numeric(first)) { // we allow munching of non-numeric word characters, so long as the first is numeric.
        diagnostic_issue_char(DIAGNOSTIC_CODE_LEXER_INVALID_NUMERICAL_SEQUENCE, this->position, first);
        return (MunchResult){.bytes_munched = 0, .valid_munch = false};
    }
    // TODO: support (') as well
    return lexer_stream_maximal_munch_text(
        this,
        buff128,
        LEXER_MAX_NUMERIC_CONSTANT_LENGTH,
        classify_numeric);
}

// Tokenizing:
Token lexer_stream_extract_token(LexerStream* const this) {
    byte buff[LEXER_MIN_STRING_LITERAL_DIFFERENTIATION_LENGTH] = { 0 }; // we create the buffer in the biggest size needed.

    byte c = lexer_stream_peek(this);
    if (lexer_char_is_EOS(c))
        return TOKEN_EOF;
    if (lexer_is_structural_character(c)) { // which is not LEXER_EOS by definition, since we already checked that.
        diagnostic_issue_char(DIAGNOSTIC_CODE_LEXER_STRUCTURAL_CHARACTER_ENCOUNTERED, this->position, (char)lexer_change_structural_status(c));
        exit(1);
    }
    while (lexer_is_char_parsing_indifferent(c))
        c = lexer_stream_pop(this);
    if (c == '\'') {
        lexer_stream_pop(this);
        const MunchResult result = lexer_stream_maximal_munch_character_literal(this, buff);
        if (!result.valid_munch) {
            diagnostic_issue_string(DIAGNOSTIC_CODE_LEXER_INVALID_CHARACTER_LITERAL_SEQUENCE, this->position, buff);
            exit(1);
        }
        if (result.bytes_munched != 1) {
            diagnostic_issue_string(DIAGNOSTIC_CODE_INVALID_INTERNAL_STATE, POSITION_EMPTY, "MunchResult was: valid_munch=true, bytes_munched != 1");
            exit(1);
        }
        return token_constant_create(TOKEN_CONSTANT_TYPE_CHARACTER, (int)buff[0]);
    }
    if (c == '\"') {
        lexer_stream_pop(this);
        lexer_stream_maximal_munch_string_literal(this, buff);
        return token_string_literal_create(buff);
    }
    if (lexer_is_char_punctuator(c)) {
        const MunchResult result = lexer_stream_maximal_munch_punctuator(this, buff);
        TokenPunctuatorID id;
        if (!result.valid_munch || (id = lexer_get_punctuator_id_by_string(buff)) == -1) {
            diagnostic_issue_string(DIAGNOSTIC_CODE_LEXER_INVALID_PUNCTUATOR_SEQUENCE, this->position, buff);
            exit(1);
        }
        return token_punctuator_create(id);
    }
    if (lexer_is_char_numeric(c)) {
        lexer_stream_maximal_munch_numeric(this, buff);
        return token_constant_create(TOKEN_CONSTANT_TYPE_INTEGER, lexer_string_to_int(buff, this->position));
    }
    if (c == '_' || lexer_is_char_alpha(c)) {
        MunchResult result = lexer_stream_maximal_munch_word(this, buff);
        if (!result.valid_munch)
            exit(1);
        TokenKeywordID id = lexer_get_keyword_id_by_string(buff);
        if (id != -1)
            return token_keyword_create(id);
        return token_identifier_create(buff);
    }
    diagnostic_issue_char(DIAGNOSTIC_CODE_LEXER_UNKNOWN_CHARACTER, this->position, (char)c);
    return TOKEN_EMPTY;
}
List* lexer_stream_extract_all_tokens(LexerStream* this) {
    List* token_list = list_create();
    Token current;
    while (!token_is_EOF(current = lexer_stream_extract_token(this))) {
        Token* new = malloc(sizeof(Token));
        *new = current;
        list_push(token_list, new);
    }
    // Now, either we reached EOS (successfully lexed the entire stream) or corrupted lexeme,
    // in which case lexer_stream_extract_token should have printed error message already.
    // Either way we return the token list.
    return token_list;
}