#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printArr(size_t n) {
    while (n--) {
        printf("%d ", rand() % 1000);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));
    size_t n;
    scanf("%zu", &n);
    printf("%zu %zu\n", n, n);
    printArr(n);
    printArr(n);

    return 0;
}
