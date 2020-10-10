#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef unsigned long long ull;

struct sieve_b {
    size_t n;
    unsigned char* s;
};

/*
 * \brief Allocate a sieve to hold all integers less or equal to \c n
 */
struct sieve_b* allocate_sieve_b(size_t n);
void free_sieve_b(struct sieve_b* sieve);
void fill_sieve_b(struct sieve_b* sv);
bool is_prime_b(struct sieve_b* sv, ull n);
