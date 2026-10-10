//
// Created by tomer on 27-Sep-26.
//

#include "SourceText.h"
#include "Utils.h"
#include "Text.h"

#include <stdlib.h>
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
 * @warning ownership of return value is transferred to the caller.
 * @param path path to a file
 * @return the substring of @b path containing the file name (anything after the last @c /).
 */
static char* get_file_name(const char* const path) {
    int last_delimiter_index = -1;
    for (int i = 0; path[i] != '\0'; ++i)
        if (is_file_path_delimiter(path[i]))
            last_delimiter_index = i;
    return utils_allocate_and_copy_string(path + last_delimiter_index + 1);
}

SourceText* source_text_create(Text* const text, const char* const name) {
    if (text == NULL || name == NULL)
        return NULL;
    SourceText* source_text = utils_xmalloc(sizeof (SourceText));
    *source_text = (SourceText){
        .name = utils_allocate_and_copy_string(name),
        .data = text
    };
    return source_text;
}
SourceText* source_text_create_from_path(const char* const path) {
    char* const file_name = get_file_name(path);
    SourceText* const to_return = source_text_create(text_create_from_path(path), file_name);
    free(file_name);
    return to_return;
}
void source_text_destroy(SourceText* const this) {
    text_destroy(this->data);
    free(this->name);
    free(this);
}
bool source_text_is_index_in_bounds(const SourceText* this, const int index) {
    return 0 <= index && index < this->data->length;
}

void source_text_list_destroy(void* this) {
    source_text_destroy(this);
}