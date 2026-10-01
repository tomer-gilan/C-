//
// Written by Claude on 01-Oct-26.
//

#include <stdlib.h>

#include "Harness.h"
#include "List.h"

static void test_list_create_is_empty(void) {
    List* const list = list_create(free);

    CHECK(list_is_empty(list));
    CHECK(list_get_size(list) == 0);

    list_destroy(list);
}

int main(void) {
    RUN_TEST(test_list_create_is_empty);
    return harness_finish();
}
