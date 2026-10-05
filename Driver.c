//
// Created by tomer on 27-Sep-26.
//

#include <stdlib.h>

#include "Driver.h"
#include "Lexer.h"
#include "Utils.h"

Driver* driver_create(const int argc, const char* const* argv) {
    if (argc <= 1) {
        // TODO: issue a diagnostic "missing source file". note that currently there's no way to do it without a context.
        exit(1); // fatal diagnostic, so exit(1).
    }

    SourceText* const source_text = source_text_create_from_path(argv[1]); // TODO: change hard-wiring to command line argument parsing.
    if (source_text == NULL)
        return NULL; // as the docs specify.
    Context* const context = context_create(source_text);

    Driver* driver = utils_xmalloc(sizeof (Driver));
    *driver = (Driver){
        .argc = argc,
        .argv = argv,
        .context = context
    };
    return driver;
}
void driver_destroy(Driver* this) {
    context_destroy(this->context);
    free(this);
}
List* driver_compile(Driver* const this, const int source_text_index) {
    Lexer* lexer = lexer_create(this->context, source_text_index);
    List* const tokens = lexer_lex_source_text(lexer);
    lexer_destroy(lexer);
    return tokens;
}
