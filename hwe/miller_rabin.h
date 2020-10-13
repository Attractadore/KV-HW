#pragma once

#include <stdbool.h>

typedef unsigned long long ull;

/**
 * \brief Use the Miller-Rabin test to check if a number is prime
 *
 * \param[in] n The number to check for primality
 *
 * \return \c true is \c n is prime, \c false otherwise
 */
bool miller_rabin(ull n);

bool is_prime_mr(ull n);
