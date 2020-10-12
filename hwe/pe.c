#include "pe.h"
#include "binary_op.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

struct sieve_b* allocate_sieve_b(size_t n) {
    struct sieve_b* sieve = calloc(1, sizeof(*sieve));
    if (sieve) {
        sieve->n = n / CHAR_BIT + 1;
        sieve->s = calloc(sieve->n, sizeof(*sieve->s));
        if (!sieve->s) {
            free(sieve);
            sieve = NULL;
        }
    }
    return sieve;
}

void free_sieve_b(struct sieve_b* sieve) {
    if (!sieve) {
        return;
    }
    free(sieve->s);
    free(sieve);
}

void fill_sieve_b(struct sieve_b* sv) {
    assert(sv);
    assert(sv->n);
    assert(sv->s);

    ull byte, bit;
    sv->s[0] = 3u;

    for (ull i = 4; i < sv->n * (ull) CHAR_BIT; i += 2) {
        get_byte_bit(i, &byte, &bit);
        set_bit(sv->s + byte, bit);
    }

    for (ull i = 3; i * i < sv->n * (ull) CHAR_BIT; i += 2) {
        get_byte_bit(i, &byte, &bit);
        if (read_bit(sv->s + byte, bit)) {
            continue;
        }
        for (ull j = i; i * j < sv->n * (ull) CHAR_BIT; j += 2) {
            get_byte_bit(i * j, &byte, &bit);
            set_bit(sv->s + byte, bit);
        }
    }
}

bool is_prime_b(struct sieve_b* sv, ull n) {
    assert(sv);
    ull byte, bit;
    get_byte_bit(n, &byte, &bit);
    assert(byte < sv->n);
    return !read_bit(sv->s + byte, bit);
}
