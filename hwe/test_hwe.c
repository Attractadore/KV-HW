#include "hwe.h"
#include "pe.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

int main() {
    ull n = UINT_MAX;

    struct sieve_t* sieve = allocate_sieve(n);
    struct sieve_b* bsieve = allocate_sieve_b(n);
    if (!sieve || !bsieve) {
        free_sieve(sieve);
        free_sieve_b(bsieve);
        return -1;
    }

    printf("Fill new sieve\n");
    fill_sieve(sieve);
    printf("Fill old sieve\n");
    fill_sieve_b(bsieve);

    ull tested_cases = 10;
    for (ull i = 0; i <= n; i++) {
        if (i == tested_cases) {
            printf("Tested first %llu numbers\n", i);
            tested_cases *= 10;
        }
        bool ps = is_prime(sieve, i);
        bool pb = is_prime_b(bsieve, i);
        if (ps != pb) {
            printf("Test failed for %llu: new sieve returned %d, old sieve returned %d\n", i, ps, pb);
            free_sieve(sieve);
            free_sieve_b(bsieve);
            return -1;
        }
    }

    printf("All %llu tests passed\n", n);

    free_sieve(sieve);
    free_sieve_b(bsieve);

    return 0;
}
