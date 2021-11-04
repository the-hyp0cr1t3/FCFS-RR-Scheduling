#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define C2_TXT "c2.txt"
#define C3_TXT "c3.txt"
#define N 1000000     // 1 million

void run_C1(int n) {
    /* Find the sum of n random numbers and print it */

    /* Note:
        rand() returns a pseudo-random number in the range of 0 to RAND_MAX.
        RAND_MAX is a constant whose default value may vary between implementations
        but it is granted to be at least 32767.
            TODO: possible issue with problem constraints? numbers have to be [1, 10^6].
    */

    long long sum = 0;
    for(int i = 0; i < n; i++)
        sum += rand() % N;

    printf("sum = %lld\n", sum);
    fflush(stdout);
}

void run_C2(int n) {
    /* Read n integers from c2.txt and print them */

    FILE *c2f;

    // File not found
    if(!(c2f = fopen(C2_TXT, "r"))) {
        perror(C2_TXT);
        exit(1);
    }

    int x, cnt = 0;
    while(fscanf(c2f, "%d", &x) && cnt++ < n) {
        printf("%d\n", x);
        fflush(stdout);

        if(feof(c2f)) break;
    }

    // File does not contain at least n integers
    if(cnt < n) {
        // Prints the numbers in the file even if cnt < n. Is it an issue?
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C2_TXT, n, cnt);
        exit(2);
    }

    fclose(c2f);

    printf("Done Printing\n");
    fflush(stdout);
}

void run_C3(int n) {
    /* Read n integers from c3.txt and print the sum */

    FILE *c3f;

    // File not found
    if(!(c3f = fopen(C3_TXT, "r"))) {
        perror(C3_TXT);
        exit(1);
    }

    long long sum = 0;
    int x, cnt = 0;
    while(fscanf(c3f, "%d", &x) && cnt++ < n) {
        sum += x;
        if(feof(c3f)) break;
    }

    // File does not contain at least n integers
    if(cnt < n) {
        // Prints the numbers in the file even if cnt < n. Is it an issue?
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C3_TXT, n, cnt);
        exit(2);
    }

    fclose(c3f);

    printf("sum = %lld\n", sum);
    fflush(stdout);
}

int main() {
    srand(time(NULL));
    run_C1(N);
    run_C2(3);
    run_C3(10);
}