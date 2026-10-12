
#include <stdio.h>

#include "Driver.h"

int main(const int argc, const char* const* const argv) {
    Driver* const driver = driver_create(argc, argv);

    List* const tokens = driver_compile(driver, 0);
    driver_issue_diagnostics(driver);

    list_destroy(tokens);
    driver_destroy(driver);

    return 0;
}
