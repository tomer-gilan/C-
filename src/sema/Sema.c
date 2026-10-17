//
// Created by tomer on 16-Oct-26.
//

#include "Sema.h"

#include "Type.h"

struct Sema {
    Context* context;
    const Type** types;
};
