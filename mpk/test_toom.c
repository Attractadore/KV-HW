#include "polynome.c"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

enum {
    TEST_ITER = 10000,
    TEST_SIZE = 100,
};

typedef PolynomeType (*FillFunction)();

PolynomeType zeroFill() {
    return 0;
}

PolynomeType randomFill() {
    // Overflow spares no one
    return rand() % 1000;
}

void fill(Polynome* const poly, const FillFunction func) {
    for (size_t i = 0; i < poly->len; i++) {
        poly->coefs[i] = func();
    }
}

bool checkEqual(Polynome const* lhs, Polynome const* rhs) {
    if (lhs->len != rhs->len) {
        return false;
    }
    for (size_t i = 0; i < lhs->len; i++) {
        if (lhs->coefs[i] != rhs->coefs[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    srand(time(NULL));

    for (size_t i = 0; i < 3; i++) {
        bool b_err = false;
        const size_t local_test_size = TEST_SIZE + i;
        printf("Running %d test iterations for size %zu\n", TEST_ITER, local_test_size);

        Polynome* poly_1 = polynomeAlloc(local_test_size - 1);
        Polynome* poly_2 = polynomeAlloc(local_test_size - 1);
        if (!poly_1 || !poly_2) {
            b_err = true;
            goto free_some;
        }

        Polynome* base_res = polynomeAlloc(polynomeMulDegree(poly_1, poly_2));
        Polynome* toom_res = polynomeAlloc(polynomeMulDegree(poly_1, poly_2));
        if (!base_res || !toom_res) {
            b_err = true;
            goto free_all;
        }

        for (size_t k = 0; k < TEST_ITER; k++) {
            if (!(k % 100)) {
                printf("%zu\n", k);
            }
            fill(poly_1, randomFill);
            fill(poly_2, randomFill);
            fill(base_res, zeroFill);
            fill(toom_res, zeroFill);
            polynomeMulBaseInp(poly_1, poly_2, base_res);
            if (!polynomeMulToomInp(poly_1, poly_2, toom_res)) {
                b_err = true;
                goto free_all;
            }
            if (!checkEqual(base_res, toom_res)) {
                fprintf(stderr, "Failed to multiply polynomes using Toom-Cook:\n");
                fprintf(stderr, "Left poly:\n");
                polynomeWrite(stderr, poly_1);
                fprintf(stderr, "Right poly:\n");
                polynomeWrite(stderr, poly_2);
                fprintf(stderr, "Expected:\n");
                polynomeWrite(stderr, base_res);
                fprintf(stderr, "Got:\n");
                polynomeWrite(stderr, toom_res);
                b_err = true;
                goto free_all;
            }
        }

    free_all:
        polynomeFree(base_res);
        polynomeFree(toom_res);
    free_some:
        polynomeFree(poly_1);
        polynomeFree(poly_2);

        if (b_err) {
            return -1;
        }
    }

    return 0;
}
