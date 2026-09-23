//
// Created by tomer on 22-Sep-26.
//

#include "Lexer.h"

#include "Utils.h"

LexerStream* lexer_stream_create(const char* data) {
    LexerStream* lexer_stream = malloc(sizeof(lexer_stream));
    lexer_stream->SPI = 0;
    lexer_stream->data = utils_allocate_and_copy_string(data);
    return lexer_stream;
}
void lexer_stream_destroy(LexerStream* handle) {
    free(handle->data);
    free(handle);
}
char lexer_stream_peek(const LexerStream* handle) {
    return handle->data[handle->SPI];
}
char lexer_stream_pop(LexerStream* handle) {
    return handle->data[handle->SPI++];
}
Token* lexer_stream_extract_token(LexerStream* handle) {}