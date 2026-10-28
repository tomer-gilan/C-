//
// Created by tomer on 21-Oct-26.
//

#ifndef C_COMPILER_PRINTER_H
#define C_COMPILER_PRINTER_H
#include "Text.h"

typedef struct Printer Printer;

// RAII:

Printer* printer_create(void);
void printer_destroy(Printer* this);

// API:

void printer_indent(Printer* this);
void printer_dedent(Printer* this);
Text* printer_detach_text(Printer* this);

// Serialization:

void printer_print_text(Printer* this, const Text* text);
void printer_print_text_view(Printer* this, TextView text);
void printer_print_char(Printer* this, char c);
void printer_print_int(Printer* this, int n);
void printer_print_new_line(Printer* this);

#endif //C_COMPILER_PRINTER_H
