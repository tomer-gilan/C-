//
// Written by Claude on 10-Oct-26.
//

// mkstemp and fdopen are POSIX, not ISO C. Under -std=c11 the headers hide them unless this is defined before the
// first #include.
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Harness.h"
#include "SourceText.h"

/**
 * Creates a file in /tmp with the given contents.
 * @param path_template a path ending in @c XXXXXX , overwritten with the real path (see @c mkstemp ).
 * @param contents the text to write, '\0'-terminated.
 * @retval true if the file was created and written.
 */
static bool create_temp_file(char* const path_template, const char* const contents) {
    const int descriptor = mkstemp(path_template);
    if (descriptor == -1)
        return false;
    FILE* const file = fdopen(descriptor, "wb");
    if (file == NULL)
        return false;
    const bool written = fputs(contents, file) >= 0;
    fclose(file);
    return written;
}

/**
 * Leak regression test (testing-roadmap.md, Step 6): @c source_text_create_from_path gets the file name from
 * @c get_file_name , which allocates, and @c source_text_create copies it again, so the first copy is never freed.
 * Invisible in a normal run; LeakSanitizer reports it (on by default in Debug builds) until it is fixed.
 */
static void test_source_text_create_from_path_valid(void) {
    char path[] = "/tmp/c_compiler_test_XXXXXX";
    if (!CHECK(create_temp_file(path, "int x;")))
        return;

    SourceText* const source_text = source_text_create_from_path(path);
    if (CHECK(source_text != NULL)) {
        CHECK(strcmp(source_text->name, strrchr(path, '/') + 1) == 0); // the name is the part after the last '/'
        CHECK(source_text->data->length == 6);
        CHECK(memcmp(source_text->data->data, "int x;", 6) == 0);
        source_text_destroy(source_text);
    }
    remove(path);
}

/**
 * Leak regression test (testing-roadmap.md, Step 6): for an invalid path, @c text_create_from_path returns
 * @c NULL , so @c source_text_create returns @c NULL without ever using (or freeing) the copy of the file name.
 */
static void test_source_text_create_from_path_invalid(void) {
    SourceText* const source_text = source_text_create_from_path("/c_compiler_no_such_directory/no_such_file.c");
    CHECK(source_text == NULL);
    if (source_text != NULL)
        source_text_destroy(source_text);
}

int main(const int argc, char** const argv) {
    harness_init(argc, argv);
    RUN_TEST(test_source_text_create_from_path_valid);
    RUN_TEST(test_source_text_create_from_path_invalid);
    return harness_finish();
}
