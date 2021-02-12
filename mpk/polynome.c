#include "polynome.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Polynome* polynomeAlloc(const size_t degree) {
    const size_t len = degree + 1;
    const size_t alloc_size = sizeof(Polynome) + sizeof(PolynomeType[len]);
    Polynome* const poly = calloc(alloc_size, sizeof(unsigned char));
    if (!poly) {
        return NULL;
    }
    poly->len = len;
    poly->coefs = (PolynomeType*) (poly + 1);
    return poly;
}

void polynomeFree(Polynome* const poly) {
    free(poly);
}

Polynome* polynomeCoefDup(Polynome const* const poly, const size_t start_degree, const size_t end_degree) {
    assert(poly);
    assert(poly->len > 0);
    assert(end_degree >= start_degree);
    assert(end_degree < poly->len);

    Polynome* const new_poly = polynomeAlloc(end_degree - start_degree);
    if (!new_poly) {
        return NULL;
    }
    memcpy(new_poly->coefs, poly->coefs + start_degree, sizeof(PolynomeType[new_poly->len]));

    return new_poly;
}

#define DEFINE_PAIRWISE_INP_FUNC(func_name, op)                           \
    Polynome* func_name(Polynome* const lhs, Polynome const* const rhs) { \
        assert(lhs);                                                      \
        assert(lhs->len > 0);                                             \
        assert(rhs);                                                      \
        assert(rhs->len > 0);                                             \
        assert(lhs->len >= rhs->len);                                     \
                                                                          \
        for (size_t i = 0; i < rhs->len; i++) {                           \
            lhs->coefs[i] = lhs->coefs[i] op rhs->coefs[i];               \
        }                                                                 \
        return lhs;                                                       \
    }

DEFINE_PAIRWISE_INP_FUNC(polynomeAddInp, +)
DEFINE_PAIRWISE_INP_FUNC(polynomeSubInp, -)

#define DEFINE_MUL_FUNC(func_name, func_inp_name)                               \
    Polynome* func_name(Polynome const* const lhs, Polynome const* const rhs) { \
        assert(lhs);                                                            \
        assert(lhs->len > 0);                                                   \
        assert(rhs);                                                            \
        assert(rhs->len > 0);                                                   \
                                                                                \
        Polynome* const res = polynomeAlloc(polynomeMulDegree(lhs, rhs));       \
        if (!res) {                                                             \
            return NULL;                                                        \
        }                                                                       \
        return func_inp_name(lhs, rhs, res);                                    \
    }

DEFINE_MUL_FUNC(polynomeMul, polynomeMulInp)
DEFINE_MUL_FUNC(polynomeMulBase, polynomeMulBaseInp)
DEFINE_MUL_FUNC(polynomeMulKar, polynomeMulKarInp)

enum {
    KAR_MIN = 32
};

Polynome* polynomeMulInp(Polynome const* const lhs, Polynome const* const rhs, Polynome* const restrict res) {
    assert(lhs);
    assert(lhs->len > 0);
    assert(rhs);
    assert(rhs->len > 0);
    assert(res);
    assert(res->len >= polynomeMulDegree(lhs, rhs));

    if (lhs->len < KAR_MIN || rhs->len < KAR_MIN) {
        return polynomeMulBaseInp(lhs, rhs, res);
    }

    return polynomeMulKarInp(lhs, rhs, res);
}

Polynome* polynomeMulBaseInp(Polynome const* const lhs, Polynome const* const rhs, Polynome* const restrict res) {
    assert(lhs);
    assert(lhs->len > 0);
    assert(rhs);
    assert(rhs->len > 0);
    assert(res);
    assert(res->len >= polynomeMulDegree(lhs, rhs));

    const size_t lhs_degree = lhs->len - 1;
    const size_t rhs_degree = rhs->len - 1;

    for (size_t i = 0; i <= lhs_degree; i++) {
        for (size_t j = 0; j <= rhs_degree; j++) {
            res->coefs[i + j] += lhs->coefs[i] * rhs->coefs[j];
        }
    }

    return res;
}

Polynome* polynomeMulKarInp(Polynome const* const lhs, Polynome const* const rhs, Polynome* const restrict res) {
    assert(lhs);
    assert(lhs->len > 0);
    assert(rhs);
    assert(rhs->len > 0);
    assert(res);
    assert(res->len >= polynomeMulDegree(lhs, rhs));

    const size_t half_len = ((lhs->len <= rhs->len) ? (lhs->len) : (rhs->len)) / 2;
    const size_t lhs_degree = lhs->len - 1;
    const size_t rhs_degree = rhs->len - 1;

    Polynome* const A = polynomeCoefDup(lhs, half_len, lhs_degree);

    const Polynome B = {
        half_len,
        lhs->coefs,
    };

    Polynome* const C = polynomeCoefDup(rhs, half_len, rhs_degree);

    const Polynome D = {
        half_len,
        rhs->coefs,
    };

    Polynome AC = {
        polynomeMulDegree(A, C) + 1,
        res->coefs + 2 * half_len,
    };

    Polynome BD = {
        polynomeMulDegree(&B, &D) + 1,
        res->coefs,
    };

    if (!A || !C) {
        goto fail;
    }

    polynomeMulInp(A, C, &AC);
    polynomeMulInp(&B, &D, &BD);

    polynomeAddInp(A, &B);
    polynomeAddInp(C, &D);

    Polynome* const tmp = polynomeMul(A, C);
    if (!tmp) {
        goto fail;
    }
    polynomeSubInp(tmp, &AC);
    polynomeSubInp(tmp, &BD);

    Polynome res_tmp = {
        tmp->len,
        res->coefs + half_len,
    };

    polynomeAddInp(&res_tmp, tmp);

    polynomeFree(A);
    polynomeFree(C);
    polynomeFree(tmp);

    return res;

fail:
    polynomeFree(A);
    polynomeFree(C);

    return NULL;
}

size_t polynomeMulDegree(Polynome const* const lhs, Polynome const* const rhs) {
    return lhs->len + rhs->len - 2;
}

size_t polynomeMaxDegree(Polynome const* const poly) {
    assert(poly);
    assert(poly->len > 0);

    for (size_t i = poly->len - 1; i > 0; i--) {
        if (poly->coefs[i]) {
            return i;
        }
    }

    return 0;
}

Polynome* polynomeRead(FILE* const file, const size_t len) {
    assert(file);
    assert(len > 0);

    const size_t degree = len - 1;
    Polynome* poly = polynomeAlloc(degree);
    if (!poly) {
        return NULL;
    }
    for (size_t i = 0; i < len; i++) {
        if (fscanf(file, "%" POLYNOME_SCAN_FORMAT, &poly->coefs[i]) != 1) {
            polynomeFree(poly);
            return NULL;
        }
    }

    return poly;
}

void polynomeWrite(FILE* const restrict file, Polynome const* const poly) {
    assert(file);
    assert(poly);

    const size_t max_degree = polynomeMaxDegree(poly);
    for (size_t i = 0; i < max_degree; i++) {
        fprintf(file, "%" POLYNOME_PRINT_FORMAT " ", poly->coefs[i]);
    }
    fprintf(file, "%" POLYNOME_PRINT_FORMAT "\n", poly->coefs[max_degree]);
}
