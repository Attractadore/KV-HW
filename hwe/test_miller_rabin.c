#include "hwe.h"
#include "miller_rabin.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

bool test_primality(ull v, struct sieve_t* sieve) {
    bool bps = is_prime(sieve, v);
    bool bpmr = is_prime_mr(v);
    if (bps != bpmr) {
        fprintf(stderr, "Test failed for %llu: sieve returned %d, Miller-Rabin test returned %d\n", v, bps, bpmr);
        return false;
    }
    return true;
}

int main() {
    ull num_tests = 1e8;

    struct sieve_t* sieve = allocate_sieve(num_tests);
    if (!sieve) {
        free_sieve(sieve);
        return -1;
    }

    printf("Fill sieve\n");
    fill_sieve(sieve);
    printf("Sieve filled\n");

    for (ull i = 0; i < num_tests; i++) {
        if (!test_primality(i, sieve)) {
            free_sieve(sieve);
            return -1;
        }
        if (((i + 1) * 10) % num_tests == 0) {
            printf("%llu%% tests passed\n", (i + 1) * 10 / num_tests * 10);
        }
    }

    fprintf(stderr, "All tests passed\n");

    free_sieve(sieve);

    return 0;
}
