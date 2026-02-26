#include <stddef.h>
#include <stdio.h>

void* malloc(size_t size);

int main (void) {
    for (int i = 0; i < 150; ++i) {
        malloc(50);
    }
}