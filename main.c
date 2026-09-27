
#include <stdio.h>

int main() {
    int a = 0, b = 0;
    int c = a++ + ++b;

    printf("%d\n", c);
    return 0;
}