#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VAL (int)(1e6)
#define C2_TXT "c2.txt"
#define C3_TXT "c3.txt"

double get_time_diff(struct timespec st, struct timespec et) {
    return (double)(et.tv_sec - st.tv_sec) + ((double)(et.tv_nsec - st.tv_nsec) / 1000000000L);
}

long long run_C1(int n) {
    /* Return the sum of n random numbers */
    /* Note:
        rand() returns a pseudo-random number in the range of 0 to RAND_MAX.
        RAND_MAX is a constant whose default value may vary between implementations
        but it is granted to be at least 32767.
            TODO: possible issue with problem constraints? numbers have to be [1, 10^6].
    */

    struct timespec st, et;

    if (timespec_get(&st, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    long long sum = 0;
    for (int i = 0; i < n; i++)
        sum += rand() % MAX_VAL + 1;

    if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    printf("[1] execution time: %lf\n", get_time_diff(st, et));

    return sum;
}

const char *run_C2(int n) {
    /* Read n integers from c2.txt and return "Done printing" */

    FILE *c2f;

    // File not found
    if (!(c2f = fopen(C2_TXT, "r"))) {
        perror(C2_TXT);
        exit(1);
    }

    struct timespec st, et;

    if (timespec_get(&st, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    int x, cnt = 0;
    while (fscanf(c2f, "%d", &x) && cnt++ < n) {
        printf("%d\t", x);

        if (feof(c2f)) break;
    }
    
    fflush(stdout);

    // File does not contain at least n integers
    if (cnt < n) {
        // Prints the numbers in the file even if cnt < n. Is it an issue?
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C2_TXT, n, cnt);
        exit(2);
    }

    if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    printf("[2] execution time: %lf\n", get_time_diff(st, et));

    fclose(c2f);
    return "Done printing";
}

long long run_C3(int n) {
    /* Read n integers from c3.txt and return the sum */

    FILE *c3f;

    // File not found
    if (!(c3f = fopen(C3_TXT, "r"))) {
        perror(C3_TXT);
        exit(1);
    }

    struct timespec st, et;

    if (timespec_get(&st, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    long long sum = 0;
    int x, cnt = 0;
    while (fscanf(c3f, "%d", &x) && cnt++ < n) {
        sum += x;
        if (feof(c3f)) break;
    }

    // File does not contain at least n integers
    if (cnt < n) {
        // Prints the numbers in the file even if cnt < n. Is it an issue?
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C3_TXT, n, cnt);
        exit(2);
    }

    if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    printf("[3] execution time: %lf\n", get_time_diff(st, et));

    fclose(c3f);
    return sum;
}

int main() {
    run_C1(1e6);
    run_C2(1e6);
    run_C3(1e6);
    return 0;
}