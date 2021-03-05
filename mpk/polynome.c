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

#define DEFINE_PAIRWISE_INP_FUNC(func_name, op)                                    \
    Polynome* func_name(Polynome* const restrict lhs, Polynome const* const rhs) { \
        assert(lhs);                                                               \
        assert(lhs->len > 0);                                                      \
        assert(rhs);                                                               \
        assert(rhs->len > 0);                                                      \
        assert(lhs->len >= rhs->len);                                              \
                                                                                   \
        for (size_t i = 0; i < rhs->len; i++) {                                    \
            lhs->coefs[i] = lhs->coefs[i] op rhs->coefs[i];                        \
        }                                                                          \
                                                                                   \
        return lhs;                                                                \
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

Polynome* polynomeMulKarInp2(Polynome const* const lhs, Polynome const* const rhs, Polynome* const restrict res) {
    assert(lhs);
    assert(lhs->len > 0);
    assert(rhs);
    assert(rhs->len > 0);
    assert(res);
    assert(res->len > polynomeMulDegree(lhs, rhs));
    assert(lhs->len % 2 == 0);
    assert(rhs->len % 2 == 0);

    const Polynome B = {
        lhs->len / 2,
        lhs->coefs,
    };

    const Polynome A = {
        lhs->len / 2,
        lhs->coefs + B.len,
    };

    const Polynome D = {
        rhs->len / 2,
        rhs->coefs,
    };

    const Polynome C = {
        rhs->len / 2,
        rhs->coefs + D.len,
    };

    Polynome AB = {
        A.len,
        NULL,
    };

    Polynome CD = {
        C.len,
        res->coefs,
    };

    Polynome AB_CD = {
        polynomeMulDegree(&AB, &CD) + 1,
        res->coefs + CD.len,
    };

    AB.coefs = AB_CD.coefs + AB_CD.len;

    memcpy(AB.coefs, A.coefs, sizeof(PolynomeType[AB.len]));
    polynomeAddInp(&AB, &B);

    memcpy(CD.coefs, C.coefs, sizeof(PolynomeType[CD.len]));
    polynomeAddInp(&CD, &D);

    if (!polynomeMulInp(&AB, &CD, &AB_CD)) {
        return NULL;
    }

    memset(AB.coefs, 0, sizeof(PolynomeType[AB.len]));
    memset(CD.coefs, 0, sizeof(PolynomeType[CD.len]));

    Polynome AC = {
        polynomeMulDegree(&A, &C) + 1,
        res->coefs + B.len + D.len,
    };

    Polynome BD = {
        polynomeMulDegree(&B, &D) + 1,
        res->coefs,
    };

    Polynome* tmp = polynomeMul(&A, &C);
    if (!tmp) {
        return NULL;
    }
    polynomeAddInp(&AC, tmp);
    polynomeSubInp(&AB_CD, tmp);

    memset(tmp->coefs, 0, sizeof(PolynomeType[tmp->len]));
    tmp->len = BD.len;
    if (!polynomeMulInp(&B, &D, tmp)) {
        polynomeFree(tmp);
        return NULL;
    }
    polynomeAddInp(&BD, tmp);
    polynomeSubInp(&AB_CD, tmp);

    polynomeFree(tmp);

    return res;
}

Polynome* polynomeMulKarInp(Polynome const* const lhs, Polynome const* const rhs, Polynome* const restrict res) {
    assert(lhs);
    assert(rhs);
    assert(res);
    assert(res->len > polynomeMulDegree(lhs, rhs));

    const Polynome new_lhs = {
        lhs->len - lhs->len % 2,
        lhs->coefs,
    };

    const Polynome new_rhs = {
        rhs->len - rhs->len % 2,
        rhs->coefs,
    };

    Polynome new_res = {
        polynomeMulDegree(&new_lhs, &new_rhs) + 1,
        res->coefs,
    };

    if (!polynomeMulKarInp2(&new_lhs, &new_rhs, &new_res)) {
        return NULL;
    }

    if (lhs->len % 2) {
        Polynome lhs_v = {
            1,
            lhs->coefs + lhs->len - 1,
        };
        Polynome dst = {
            rhs->len,
            res->coefs + lhs->len - 1,
        };
        polynomeMulBaseInp(&lhs_v, rhs, &dst);
    }
    if (rhs->len % 2) {
        Polynome rhs_v = {
            1,
            rhs->coefs + rhs->len - 1,
        };
        Polynome dst = {
            lhs->len,
            res->coefs + rhs->len - 1,
        };
        polynomeMulBaseInp(lhs, &rhs_v, &dst);
    }
    if (lhs->len % 2 && rhs->len % 2) {
        res->coefs[lhs->len + rhs->len - 2] = lhs->coefs[lhs->len - 1] * rhs->coefs[rhs->len - 1];
    }

    return res;
}

size_t polynomeMulDegree(Polynome const* const lhs, Polynome const* const rhs) {
    assert(lhs && lhs->len > 0);
    assert(rhs && rhs->len > 0);
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

Polynome* polynomeRead(FILE* const restrict file, const size_t len) {
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
