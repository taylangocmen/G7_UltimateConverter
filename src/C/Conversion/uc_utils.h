
#ifndef UC_UTILS_H
#define UC_UTILS_H

#define BOFF0(p) ((p >> 0) & 0xFF)
#define BOFF1(p) ((p >> 8) & 0xFF)
#define BOFF2(p) ((p >> 16) & 0xFF)
#define BOFF3(p) ((p >> 24) & 0xFF)

#define BINARY(x)    (print_binary(sizeof(x), &x))

enum UC_FORMAT {
    bmp,
    png,
};

void abort_(const char* s, ...);

unsigned to_power(unsigned base, unsigned power);

void print_binary(size_t const size, void const* const ptr);

unsigned get_bytes(unsigned i, unsigned j, unsigned char* buffer);

void print_unsigned_bytes(unsigned x, unsigned l);

unsigned round_(float i);

#endif /* UC_UTILS_H */
