
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "uc_utils.h"

void abort_(const char* s, ...) {
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);

    abort();
}

unsigned to_power(unsigned base, unsigned power) {
    if (power == 0 || base == 1)
        return 1;

    unsigned res = base;
    unsigned i;

    for (i = 1; i < power; i++)
        res *= base;

    return res;
}

void print_binary(size_t const size, void const * const ptr) {
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i = size - 1; i >= 0; i--)
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }

    printf("\n");

    return;
}

unsigned get_bytes(unsigned i, unsigned j, unsigned char* buffer) {
    unsigned sum = 0;
    unsigned k = 0;

    if (j > i) {
        for (k = i; k < j; k++) {
            sum *= 256;
            sum += (unsigned) buffer[k];
        }
    } else if (i > j) {
        for (k = i - 1; k > j - 1; k--) {
            sum *= 256;
            sum += (unsigned) buffer[k];
        }
    } else
        abort_("Tried to get 0 bytes i: %u j: % u file: %s", i, j, buffer);

    return sum;
}

void print_unsigned_bytes(unsigned x, unsigned l){
    if(l > 0) printf("0(%8u):  %02X\n", x, BOFF0(x));
    if(l > 1) printf("1(%8u):  %02X\n", x, BOFF1(x));
    if(l > 2) printf("2(%8u):  %02X\n", x, BOFF2(x));
    if(l > 3) printf("3(%8u):  %02X\n", x, BOFF3(x));
}
