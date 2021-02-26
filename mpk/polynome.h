#pragma once

#include <stdio.h>

#if 1
typedef long long PolynomeType;
#define POLYNOME_SCAN_FORMAT "lld"
#define POLYNOME_PRINT_FORMAT "lld"
#else
typedef int PolynomeType;
#define POLYNOME_SCAN_FORMAT "d"
#define POLYNOME_PRINT_FORMAT "d"
#endif

typedef struct {
    size_t len;
    PolynomeType* coefs;
} Polynome;

Polynome* polynomeAlloc(size_t degree);
void polynomeFree(Polynome* poly);

Polynome* polynomeMul(Polynome const* lhs, Polynome const* rhs);
Polynome* polynomeMulInp(Polynome const* lhs, Polynome const* rhs, Polynome* res);
Polynome* polynomeMulBase(Polynome const* lhs, Polynome const* rhs);
Polynome* polynomeMulBaseInp(Polynome const* lhs, Polynome const* rhs, Polynome* res);
Polynome* polynomeMulKar(Polynome const* lhs, Polynome const* rhs);
Polynome* polynomeMulKarInp(Polynome const* lhs, Polynome const* rhs, Polynome* res);

size_t polynomeMulDegree(Polynome const* lhs, Polynome const* rhs);
size_t polynomeMaxDegree(Polynome const* poly);

Polynome* polynomeRead(FILE* file, size_t len);
void polynomeWrite(FILE* file, Polynome const* poly);
