#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef unsigned long long ull;

struct sieve_t {
    size_t n;
    unsigned char* mod1;
    unsigned char* mod5;
};

/*
 * \brief Allocate a sieve to hold all integers less or equal to \c n
 */
struct sieve_t* allocate_sieve(size_t n);
void free_sieve(struct sieve_t* sieve);
void fill_sieve(struct sieve_t* sv);
bool is_prime(struct sieve_t* sv, ull n);
