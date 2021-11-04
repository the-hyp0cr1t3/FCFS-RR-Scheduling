#include "OS_assn.h"

/* TODO: Fork and use pipes; Call respective functions */

int main() {
    srand(time(NULL));

    long long sum_c1 = run_C1(1000);
    printf("c1 sum = %lld\n", sum_c1);
    fflush(stdout);

    const char *msg = run_C2(3);
    printf("c2 msg = %s\n", msg);
    fflush(stdout);

    long long sum_c3 = run_C3(10);
    printf("c3 sum = %lld\n", sum_c3);
    fflush(stdout);
}

// gcc partA.c child_functions.c -o a.out