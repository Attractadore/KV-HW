#include "modmath.h"

#include <assert.h>
#include <limits.h>

ull mulm(ull x, ull y, ull m) {
    assert(m > 0);

    if (x < ULLONG_MAX / y) {
        return (x * y) % m;
    }

    ull res = 0;
    ull acc = x % m;
    while (y > 0) {
        if (y & 1u) {
            res = (res + acc) % m;
        }
        acc = (acc << 1) % m;
        y >>= 1;
    }
    return res;
}

ull powm(ull x, ull y, ull m) {
    assert(m > 0);

    ull res = 1;
    ull acc = x % m;
    while (y > 0) {
        if (y & 1u) {
            res = mulm(res, acc, m);
        }
        acc = mulm(acc, acc, m);
        y >>= 1;
    }
    return res;
}
