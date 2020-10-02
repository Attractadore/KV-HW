#include <assert.h>

#define NUM_FIBS (sizeof(int) * 8 * 2)

long long fib(int n)
{
    static long long cache[NUM_FIBS] = { };

    assert(n >= 0);
    assert(n < NUM_FIBS);

    if (n < 2) {
        return n;
    }
    if (!cache[n]) {
        cache[n] = fib(n - 1) + fib(n - 2);
    }
    return cache[n];
}

int closest_less_equal_fib_n(int x)
{
    int fib_n = 0;

    assert(x >= 0);

    while (fib_n < NUM_FIBS && fib(fib_n + 1) <= x) {
        fib_n++;
    }
    return fib_n;
}

long long smallest_fib_decompose(long long x)
{
    int fib_n = 0;

    assert(x >= 0);

    while (x > 0) {
        fib_n = closest_less_equal_fib_n(x);
        x -= fib(fib_n);
    }

    return fib(fib_n);
}

int next_turn(int total, int possible)
{
    long long best_move = smallest_fib_decompose(total);
    if (best_move > possible) {
        return 1;
    }
    return best_move;
}
