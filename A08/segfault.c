// segfault.c
#include <stdio.h>

int main() {
    int *p = NULL;
    *p = 42;  // Dereference a NULL pointer to cause segmentation fault
    return 0;
}