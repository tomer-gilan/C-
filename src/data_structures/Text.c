//
// Created by tomer on 05-Oct-26.
//

#include "data_structures/Text.h"
#include "Utils.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// -------- private ----------- //
static bool text_has_met_capacity(const Text* this) {
    assert(this != NULL && "list handle was NULL");
    return this->length == this->capacity;
}
static void text_expand(Text* this) {
    assert(this != NULL && "list handle was NULL");
    if (this->capacity == 0)
        this->capacity = 1;
    else
        this->capacity *= 2;
    this->data = utils_xrealloc(this->data, sizeof(char) * this->capacity);
}

Text* text_create_empty(void) {
    return text_create_from_buffer("", 0);
}

Text* text_create_from_view(const TextView text_view) {
    return text_create_from_buffer(text_view.data, text_view.length);
}

Text* text_create_from_buffer(const char* buffer, const int length) {
    Text* const text_view = utils_xmalloc(sizeof(Text));
    *text_view = (Text){
        .data = utils_allocate_and_copy_buffer(buffer, length),
        .length = length,
        .capacity = length
    };
    return text_view;
}
Text* text_create_from_path(const char* const path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END); // jump to the end...
    const long size = ftell(file);      // ...position = file size in bytes
    assert(size != -1 && "Problem with file");
    rewind(file);                       // back to the start

    char* buffer = utils_xmalloc(size + 1);    // +1 for a '\0' terminator
    long const read = (long)fread(buffer, 1, size, file); // trust fread's count, not ftell's
    if (read != (long)size) {         // short read = I/O error or file changed
        free(buffer);
        fclose(file);
        return NULL;                    // (or report a diagnostic)
    }
    buffer[read] = '\0';                // handy for debugging/printing
    fclose(file);

    Text* const text = utils_xmalloc(sizeof (Text));
    *text = (Text){
        .length = (int)read,
        .data = buffer,
        .capacity = (int)read
    };
    return text;
}
bool text_write_to_path(const Text* const this, const char* const path) {
    // this helper was written by Claude:
    assert(this != NULL);
    assert(path != NULL);

    FILE* const file = fopen(path, "wb");   // "wb": creates or truncates; no '\n' translation on any platform
    if (file == NULL)
        return false;

    const size_t written = fwrite(this->data, 1, (size_t)this->length, file);
    const bool closed = fclose(file) == 0;  // buffered data is only flushed here, so a full disk can show up here only
    if (written != (size_t)this->length || !closed) {
        remove(path);                       // don't leave a truncated file behind
        return false;
    }
    return true;
}
Text* text_create_from_int(const int number) {
    // this helper was written by Claude:
    char buffer[12];   // "-2147483648" is 11 characters, plus the '\0'
    const int written = snprintf(buffer, sizeof (buffer), "%d", number);
    assert(0 <= written && written < (int)(sizeof (buffer)));   // < size means nothing was cut off
    return text_create_from_buffer(buffer, written);
}

void text_destroy(Text* const this){
    assert(this != NULL);

    free(this->data);
    free(this);
}
bool text_is_empty(const Text* this) {
    assert(this != NULL);
    return text_view_is_empty(text_as_view(this));
}
bool text_equals(const Text* const first, const Text* const second) {
    assert(first != NULL && second != NULL);
    if (first->length != second->length)
        return false;
    const int length = first->length;
    return memcmp(first->data, second->data, length) == 0;
}
TextView text_as_view(const Text* const this) {
    return text_view_make(this->length, this->data);
}

void text_append_char(Text* const this, const char c) {
    assert(this != NULL);
    if (text_has_met_capacity(this))
        text_expand(this);
    this->data[this->length++] = c;
}
void text_append_text_view(Text* const this, const TextView view) {
    assert(this != NULL);

    for (int i = 0; i < view.length; ++i)
        text_append_char(this, view.data[i]);
}
void text_append_text(Text* const this, const Text* const other) {
    assert(this != NULL);
    assert(other != NULL);

    text_append_text_view(this, text_as_view(other));
}
void text_append_int(Text* const this, const int number) {
    assert(this != NULL);

    Text* const to_append = text_create_from_int(number);
    text_append_text(this, to_append);
    text_destroy(to_append);
}

// wrappers for List.h
bool text_list_equals(const void* first, const void* second) {
    return text_equals(first, second);
}
void* text_list_duplicate(const void* text) {
    return text_create_from_buffer(
        ((const Text*)text)->data,
        ((const Text*)text)->length);
}
void text_list_destroy(void* this) {
    text_destroy(this);
}
