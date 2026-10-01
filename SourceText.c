//
// Created by tomer on 27-Sep-26.
//

#include "Utils.h"
#include "SourceText.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/**
 * @param c the character to check
 * @retval true if c is a delimiter ('\' or '/')
 * @retval false otherwise
 */
static bool is_file_path_delimiter(const char c) {
    return c == '\\' || c == '/';
}
/**
 * @warning the return value is a shallow reference to @b path.
 * @param path path to a file
 * @return the substring of @b path containing the file name (anything after the last @c /).
 */
static const char* get_file_name(const char* const path) {
    int last_delimiter_index = -1;
    for (int i = 0; path[i] != '\0'; ++i)
        if (is_file_path_delimiter(path[i]))
            last_delimiter_index = i;
    return &(path[last_delimiter_index + 1]); // returning substring AFTER last occurrence.
}

SourceText* source_text_create_from_path(const char* const path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END); // jump to the end...
    const long size = ftell(file);      // ...position = file size in bytes
    assert(size != -1 && "Problem with file");
    rewind(file);                       // back to the start

    char* buffer = utils_xmalloc(size + 1);    // +1 for a '\0' terminator
    long read = (long)fread(buffer, 1, size, file); // trust fread's count, not ftell's
    if (read != (size_t)size) {         // short read = I/O error or file changed
        free(buffer);
        fclose(file);
        return NULL;                    // (or report a diagnostic)
    }
    buffer[read] = '\0';                // handy for debugging/printing
    fclose(file);

    SourceText* source_file = utils_xmalloc(sizeof (SourceText));
    *source_file = (SourceText){
        .name = utils_allocate_and_copy_string(get_file_name(path)),
        .text = buffer,
        .length = (int)read
    };
    return source_file;
}
SourceText * source_text_create_from_buffer(const char* const buffer, const int length) {
    SourceText* source_text = utils_xmalloc(sizeof (SourceText));
    *source_text = (SourceText){
        .name = utils_allocate_and_copy_string("<from string>"),
        .text = utils_allocate_and_copy_buffer(buffer, length),
        .length = length
    };
    return source_text;
}

void source_text_destroy(SourceText* const this) {
    free(this->name);
    free(this->text);
    free(this);
}
bool source_text_is_index_in_bounds(const SourceText* this, const int index) {
    return 0 <= index && index < this->length;
}

void source_text_list_destroy(void* this) {
    source_text_destroy(this);
}