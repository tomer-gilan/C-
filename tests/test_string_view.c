//
// Written by tomer on 02-Oct-26.
//

#include <assert.h>
#include <stdlib.h>

#include "Harness.h"
#include "StringView.h"

// helpers
int equals_int(const void* first, const void* second) {
    return *((int*)first) == *((int*)second) ? 0 : 1;
}
void* duplicate_int(const void* first) {
    int* to_return = malloc(sizeof(int));
    *to_return = *((int*)first);
    return to_return;
}
// end helpers

static void test_string_view_(void) {
}

static void test_string_view_equals(void) {
    StringView sv_literal[] = {
        string_view_from_literal(""),
        string_view_from_literal("a"),
        string_view_from_literal("ab"),
        string_view_from_literal("abc"),
        string_view_from_literal("bc"),
        string_view_from_literal("b"),
        string_view_from_literal("c"),
        string_view_from_literal("ac"),
        string_view_from_literal("babc"),
        string_view_from_literal("abcc"),
        string_view_from_literal("a\0a"),
        string_view_from_literal("b\0b"),
        string_view_from_literal("c\0"),
        string_view_from_literal("\0d"),
        string_view_from_literal("\0")
    };
    StringView sv_reg[] = {
        {.data = "", .length = 0},
        {.data = "a", .length = 1},
        {.data = "ab", .length = 2},
        {.data = "abc", .length = 3},
        {.data = "bc", .length = 2},
        {.data = "b", .length = 1},
        {.data = "c", .length = 1},
        {.data = "ac", .length = 2},
        {.data = "babc", .length = 4},
        {.data = "abcc", .length = 4},
        {.data = "a\0a", .length = 3},
        {.data = "b\0b", .length = 3},
        {.data = "c\0", .length = 2},
        {.data = "\0d", .length = 2},
        {.data = "\0", .length = 1}
    };
    CHECK(sv_literal[10].length == 3); // "a\0a"
    CHECK(sv_literal[11].length == 3); // "b\0b"
    CHECK(sv_literal[12].length == 2); // "c\0"
    CHECK(sv_literal[13].length == 2); // "\0d"
    CHECK(sv_literal[14].length == 1); // "\0"

    {
        StringView sv1 = string_view_from_literal("a\0b                     c  ");
        StringView sv2 = string_view_from_literal("a\0b                     c ");
        StringView sv3 = string_view_from_literal("a\0b                     c");
        CHECK(!string_view_equals(sv1, sv2));
        CHECK(!string_view_equals(sv2, sv3));
        CHECK(!string_view_equals(sv1, sv3));
    }

    _Static_assert(sizeof(sv_literal) == sizeof(sv_reg));
    const int N = sizeof(sv_literal) / sizeof(StringView);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (i == j)
                CHECK(string_view_equals(sv_literal[i], sv_reg[j]));
            else
                CHECK(!string_view_equals(sv_literal[i], sv_reg[j]));

    {
        StringView sv1 = {.data = "abc", .length = 3};
        StringView sv2 = {.data = "ab", .length = 2};
        CHECK(!string_view_equals(sv1, sv2));
        sv1.length = 2;
        CHECK(string_view_equals(sv1, sv2));
    }
    {
        StringView sv1 = {.data = "abc", .length = 2};
        StringView sv2 = {.data = "ab", .length = 0};
        CHECK(!string_view_equals(sv1, sv2));
        sv1.length = 0;
        CHECK(string_view_equals(sv1, sv2));
    }
}
static void test_string_view_list_duplicate(void) {
    {
        StringView sv1 = {.data = "", .length = 0};
        StringView* sv_ptr = string_view_list_duplicate(&sv1);
        CHECK(string_view_equals(sv1, *sv_ptr));
        free(sv_ptr);
    }
    {
        StringView sv1 = {.data = "abc", .length = 0};
        StringView* sv_ptr = string_view_list_duplicate(&sv1);
        CHECK(string_view_equals(sv1, *sv_ptr));
        free(sv_ptr);
    }
    {
        StringView sv1 = {.data = "abc", .length = 2};
        StringView* sv_ptr = string_view_list_duplicate(&sv1);
        CHECK(string_view_equals(sv1, *sv_ptr));
        free(sv_ptr);
    }
    {
        StringView sv1 = {.data = "a\0bc", .length = 2};
        StringView* sv_ptr = string_view_list_duplicate(&sv1);
        CHECK(string_view_equals(sv1, *sv_ptr));
        free(sv_ptr);
    }
    {
        StringView sv1 = string_view_from_literal("a\0b                     c  ");
        StringView* sv_ptr = string_view_list_duplicate(&sv1);
        CHECK(string_view_equals(sv1, *sv_ptr));
        free(sv_ptr);
    }
    {
        StringView sv1 = string_view_from_literal("a\0b                     c  ");
        StringView sv2 = string_view_from_literal("a\0b                     c ");
        StringView sv3 = string_view_from_literal("a\0b                     c");

        StringView* sv_ptr1 = string_view_list_duplicate(&sv1);
        StringView* sv_ptr2 = string_view_list_duplicate(&sv2);
        StringView* sv_ptr3 = string_view_list_duplicate(&sv3);

        CHECK(string_view_equals(sv1, *sv_ptr1));
        CHECK(string_view_equals(sv2, *sv_ptr2));
        CHECK(string_view_equals(sv3, *sv_ptr3));
        free(sv_ptr1);
        free(sv_ptr2);
        free(sv_ptr3);
    }
}

int main(void) {
    RUN_TEST(test_string_view_equals);
    RUN_TEST(test_string_view_list_duplicate);
    return harness_finish();
}
