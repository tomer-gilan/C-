//
// Created by tomer on 21-Oct-26.
//

#include "AstSerializer.h"
#include "Printer.h"
#include "Utils.h"

#include <assert.h>
#include <stdlib.h>

struct AstSerializer {
    AstSerializationMode serialization_mode;
    Printer* serializer;
};

AstSerializer* ast_serializer_create(const AstSerializationMode serialization_mode) {
    AstSerializer* to_return = utils_xmalloc(sizeof (AstSerializer));
    *to_return = (AstSerializer){
        .serialization_mode = serialization_mode,
        .serializer = printer_create()
    };
    return to_return;
}
void ast_serializer_destroy(AstSerializer* const this) {
    assert(this != NULL);

    printer_destroy(this->serializer);
    free(this);
}
