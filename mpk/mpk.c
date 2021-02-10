#include "polynome.h"

int main() {
    size_t poly1_size, poly2_size;
    if (fscanf(stdin, "%zu%zu", &poly1_size, &poly2_size) != 2) {
        return -1;
    }
    Polynome* const poly1 = polynomeRead(stdin, poly1_size);
    Polynome* const poly2 = polynomeRead(stdin, poly2_size);
    if (poly1 && poly2) {
        Polynome* const poly3 = polynomeMul(poly1, poly2);
        if (poly3) {
            polynomeWrite(stdout, poly3);
        }
        polynomeFree(poly3);
    }
    polynomeFree(poly1);
    polynomeFree(poly2);

    return 0;
}
