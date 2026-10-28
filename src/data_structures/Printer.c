//
// Created by tomer on 21-Oct-26.
//

#include "Printer.h"

#include <assert.h>
#include <stdlib.h>

#include "Utils.h"

struct Printer {
    int indentation_counter;
    Text* text;
};

// Predicates

static bool printer_is_text_detached(const Printer* const this) {
    return this->text == NULL;
}

// Printings:

static void printer_print_indentations(Printer* const this) {
    assert(this != NULL);
    assert(!printer_is_text_detached(this));

    for (int i = 0; i < this->indentation_counter; ++i)
        printer_print_text_view(this, text_view_value_of_literal("    "));
}
void printer_print_new_line(Printer* const this) {
    assert(this != NULL);
    assert(!printer_is_text_detached(this));

    printer_print_char(this, '\n');
    printer_print_indentations(this);
}
void printer_print_text(Printer* const this, const Text* const text) {
    assert(this != NULL);
    assert(!printer_is_text_detached(this));

    text_append_text(this->text, text);
}
void printer_print_text_view(Printer* const this, const TextView text) {
    assert(this != NULL);
    assert(!printer_is_text_detached(this));

    text_append_text_view(this->text, text);
}
void printer_print_char(Printer* const this, const char c) {
    assert(this != NULL);
    assert(!printer_is_text_detached(this));

    text_append_char(this->text, c);
}
void printer_print_int(Printer* const this, const int n) {
    assert(this != NULL);
    assert(!printer_is_text_detached(this));

    text_append_int(this->text, n);
}

// Public API:

Printer* printer_create(void) {
    Printer* to_return = utils_xmalloc(sizeof (Printer));
    *to_return = (Printer){
        .indentation_counter = 0,
        .text = text_create_empty()
    };
    return to_return;
}
void printer_destroy(Printer* const this) {
    assert(this != NULL);

    if (!printer_is_text_detached(this))
        text_destroy(this->text);
    free(this);
}

void printer_indent(Printer* const this) {
    assert(this != NULL);

    this->indentation_counter++;
}
void printer_dedent(Printer* const this) {
    assert(this != NULL);
    assert(this->indentation_counter > 0);

    this->indentation_counter--;
}
Text* printer_detach_text(Printer* const this) {
    assert(this != NULL);
    assert(!printer_is_text_detached(this));

    Text* const to_return = this->text;
    this->text = NULL;
    return to_return;
}
