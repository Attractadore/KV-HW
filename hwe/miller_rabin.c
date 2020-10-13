#include "miller_rabin.h"
#include "modmath.h"

#include <assert.h>
#include <stdlib.h>

ull factor_d(ull n) {
    while ((n & 1u) == 0) {
        n >>= 1;
    }
    return n;
}

bool miller_rabin_iter(ull n, ull a, ull d) {
    assert(n & 1u);
    assert(a > 1);
    assert(d > 0);

    ull x = powm(a, d, n);
    if (x == 1) {
        return true;
    }
    for (ull p = d; p < n - 1; p *= 2) {
        if (x == n - 1) {
            return true;
        }
        x = mulm(x, x, n);
    }
    return false;
}

bool miller_rabin(ull n) {
    static const ull witnesses[] =
        {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    static const ull num_witnesses = sizeof(witnesses) / sizeof(*witnesses);

    assert(n & 1u);

    ull d = factor_d(n - 1);
    for (ull i = 0; i < num_witnesses && witnesses[i] < n; i++) {
        if (!miller_rabin_iter(n, witnesses[i], d)) {
            return false;
        }
    }
    return true;
}

bool is_prime_mr(ull n) {
    if (n == 2 || n == 3) {
        return true;
    }
    if (n < 2 || n % 2 == 0 || n % 3 == 0) {
        return false;
    }
    return miller_rabin(n);
}
