#include "hwe.h"
#include "binary_op.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

struct sieve_t* allocate_sieve(size_t n) {
    struct sieve_t* sieve = calloc(1, sizeof(*sieve));
    if (sieve) {
        sieve->n = n / (6 * CHAR_BIT) + 1;
        sieve->mod1 = calloc(sieve->n, sizeof(*sieve->mod1));
        sieve->mod5 = calloc(sieve->n, sizeof(*sieve->mod5));
        if (!sieve->mod1 || !sieve->mod5) {
            free(sieve->mod1);
            free(sieve->mod5);
            free(sieve);
            sieve = NULL;
        }
    }
    return sieve;
}

void free_sieve(struct sieve_t* sieve) {
    if (!sieve) {
        return;
    }
    free(sieve->mod1);
    free(sieve->mod5);
    free(sieve);
}

ull quadratic_v(ull a, ull b, ull c, ull x) {
    return a * x * x + b * x + c;
}

void fill_sieve(struct sieve_t* sv) {
    assert(sv);
    assert(sv->n);
    assert(sv->mod1);
    assert(sv->mod5);

    ull byte, bit;

    for (ull i = 0; i * i < sv->n * CHAR_BIT; i++) {
        get_byte_bit(i, &byte, &bit);

        if (!read_bit(sv->mod1 + byte, bit)) {
            ull m1 = (i + 1) * 6 + 1;
            for (ull j = quadratic_v(6, 14, 7, i); j < sv->n * (ull) CHAR_BIT; j += m1) {
                get_byte_bit(j, &byte, &bit);
                set_bit(sv->mod1 + byte, bit);
            }
            for (ull j = quadratic_v(6, 18, 12, i); j < sv->n * (ull) CHAR_BIT; j += m1) {
                get_byte_bit(j, &byte, &bit);
                set_bit(sv->mod5 + byte, bit);
            }
        }
        get_byte_bit(i, &byte, &bit);

        if (!read_bit(sv->mod5 + byte, bit)) {
            ull m5 = (i + 1) * 6 - 1;
            for (ull j = quadratic_v(6, 10, 3, i); j < sv->n * (ull) CHAR_BIT; j += m5) {
                get_byte_bit(j, &byte, &bit);
                set_bit(sv->mod1 + byte, bit);
            }
            for (ull j = quadratic_v(6, 12, 5, i); j < sv->n * (ull) CHAR_BIT; j += m5) {
                get_byte_bit(j, &byte, &bit);
                set_bit(sv->mod5 + byte, bit);
            }
        }
    }
}

bool is_prime(struct sieve_t* sv, ull n) {
    assert(sv);

    ull byte, bit, m6;

    if (n < 2) {
        return 0;
    }
    if (n == 2 || n == 3) {
        return 1;
    }

    m6 = n % 6;
    if (m6 == 1) {
        get_byte_bit(n / 6 - 1, &byte, &bit);
        assert(byte < sv->n);
        return !read_bit(sv->mod1 + byte, bit);
    }
    if (m6 == 5) {
        get_byte_bit(n / 6, &byte, &bit);
        assert(byte < sv->n);
        return !read_bit(sv->mod5 + byte, bit);
    }

    return 0;
}
