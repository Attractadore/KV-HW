#include "binary_op.h"

#include <assert.h>
#include <limits.h>

void set_bit(unsigned char *p, ull n) {
    assert(n < CHAR_BIT);

    *p |= 1u << n;
}

bool read_bit(unsigned char const *p, ull n) {
    assert(n < CHAR_BIT);

    return (*p >> n) & 1u;
}

void get_byte_bit(ull i, ull *byte, ull *bit) {
    assert(byte);
    assert(bit);

    const ull mod = CHAR_BIT - 1;

    *byte = i / CHAR_BIT;
    *bit = i & mod;
}
