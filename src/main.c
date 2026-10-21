
#include <stdlib.h>

#include "Driver.h"

int main(const int argc, const char* const* const argv) {
    Driver* const driver = driver_create(argc, argv);

    const bool succeeded = driver_compile(driver, 0);
    driver_destroy(driver);

    return succeeded ? EXIT_SUCCESS : EXIT_FAILURE;
}
