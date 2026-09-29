//
// Created by tomer on 27-Sep-26.
//

#include "SourceFile.h"

#include "Utils.h"

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
    const char* lastSlashIndex;
    for (lastSlashIndex = path; *lastSlashIndex != '\0' && !is_file_path_delimiter(*lastSlashIndex); ++lastSlashIndex);
    if (*lastSlashIndex == '\0')
        return path; // did not find any delimiters, so returning the entire string.
    return ++lastSlashIndex; // returning substring AFTER last occurrence.
}

SourceFile* source_file_create(const char* const path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL)
        return NULL;

    fseek(file, 0, SEEK_END); // jump to the end...
    const long size = ftell(file);      // ...position = file size in bytes
    rewind(file);                       // back to the start

    char* buffer = malloc(size + 1);    // +1 for a '\0' terminator
    long read = (long)fread(buffer, 1, size, file); // trust fread's count, not ftell's
    if (read != (size_t)size) {         // short read = I/O error or file changed
        free(buffer);
        fclose(file);
        return NULL;                    // (or report a diagnostic)
    }
    buffer[read] = '\0';                // handy for debugging/printing
    fclose(file);

    SourceFile* source_file = malloc(sizeof (SourceFile));
    *source_file = (SourceFile){
        .name = utils_allocate_and_copy_string(get_file_name(path)),
        .text = buffer,
        .length = (int)read
    };
    return source_file;
}
void source_file_destroy(SourceFile* const this) {
    free(this->name);
    free(this->text);
    free(this);
}
bool source_file_is_index_in_bounds(const SourceFile* this, const int index) {
    return 0 <= index && index < this->length;
}

void source_file_list_destroy(void* this) {
    source_file_destroy(this);
}