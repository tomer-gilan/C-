//
// Created by tomer on 27-Sep-26.
//

#include "Driver.h"

Driver* driver_create(const int argc, const char* const* argv) {
    const char* source_file_path = argv[1]; // TODO: change hard-wiring to command line argument parsing.
    Driver* driver = malloc(sizeof (Driver));
    *driver = (Driver){
        .argc = argc,
        .argv = argv,
        .context = context_create(source_file_path)
    };
    return driver;
}
void driver_destroy(Driver* this) {
    context_destroy(this->context);

    free(this);
}