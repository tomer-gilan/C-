//
// Created by tomer on 05-Oct-26.
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Text.h"

#include <stdio.h>

#include "Utils.h"

Text* text_create_from_buffer(const char* buffer, const int length) {
    Text* const string_view = utils_xmalloc(sizeof(Text));
    *string_view = (Text){
        .data = utils_allocate_and_copy_buffer(buffer, length),
        .length = length
    };
    return string_view;
}
Text* text_create_from_file(const char* const path) {
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
        .data = buffer
    };
    return text;
}
void text_destroy(Text* const this){
    free(this->data);
    free(this);
}
bool text_equals(const Text* const first, const Text* const second) {
    assert(first != NULL && second != NULL);
    return text_compare(first, second) == 0;
}
int text_compare(const Text* const first, const Text* const second) {
    if (first->length != second->length)
        return -1; // -1 as in not 0, i.e. not equal.
    const int length = first->length;
    return memcmp(first->data, second->data, length);
}
int text_list_equals(const void* first, const void* second) {
    return text_equals(first, second);
}
int text_list_compare(const void* first, const void* second) {
    return text_compare(first, second);
}
void* text_list_duplicate(const void* text) {
    return text_create_from_buffer(
        ((Text*)text)->data,
        ((Text*)text)->length
        );
}
