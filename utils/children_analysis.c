#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUE 1000000   // generate values in the range [1, 1e6]
#define DEFAULT_N 1000000   // default size of file 1e6
#define C2_TXT "../c2.txt"  // run gen_txt.c to generate
#define C3_TXT "../c3.txt"

struct timespec get_time();
double get_time_diff(struct timespec st, struct timespec et);

long long run_C1(int n);
const char *run_C2(int n);
long long run_C3(int n);

/* Analyse execution time of the core logic of each child process */
int main(int argc, char* argv[]) {
    srand(time(NULL));
    int n1, n2, n3;
    n1 = n2 = n3 = DEFAULT_N;

    // accepts n1, n2, n3 as command line arguements (else uses defaults)
    if (argc >= 2)
        n1 = atoi(argv[1]);

    if (argc >= 3)
        n2 = atoi(argv[2]);

    if (argc >= 4)
        n3 = atoi(argv[3]);

    run_C1(n1);
    run_C2(n2);
    run_C3(n3);

    return 0;
}


struct timespec get_time() {
    struct timespec tm;
    if (timespec_get(&tm, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    return tm;
}

double get_time_diff(struct timespec st, struct timespec et) {
    return et.tv_sec - st.tv_sec + (double)(et.tv_nsec - st.tv_nsec) / 1000000000LL;
}

/* Return the sum of n random numbers */
long long run_C1(int n) {
    /* Note:
        rand() returns a pseudo-random number in the range of 0 to RAND_MAX.
        RAND_MAX is a constant whose default value may vary between implementations
        but it is granted to be at least 32767.
    */

    long long sum = 0;
    struct timespec begin = get_time();

    { // code to be benchmarked
        for (int i = 0; i < n; i++)
            sum += rand() % MAX_VALUE + 1;
    }

    printf("[1] execution time: %lf\n", get_time_diff(begin, get_time()));

    return sum;
}

/* Read n integers from c2.txt and return "Done printing" */
const char *run_C2(int n) {

    FILE *c2f;
    // File not found
    if (!(c2f = fopen(C2_TXT, "r"))) {
        perror(C2_TXT);
        exit(1);
    }

    int x, cnt = 0;
    struct timespec begin = get_time();

    { // code to be benchmarked
        while (fscanf(c2f, "%d", &x) && cnt++ < n) {
            printf("%d\t", x);
            fflush(stdout);
            if (feof(c2f)) break;
        }
    }

    // File does not contain at least n integers
    if (cnt < n) {
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C2_TXT, n, cnt);
        exit(2);
    }

    printf("[2] execution time: %lf\n", get_time_diff(begin, get_time()));
    fclose(c2f);

    return "Done printing";
}

/* Read n integers from c3.txt and return the sum */
long long run_C3(int n) {

    FILE *c3f;
    // File not found
    if (!(c3f = fopen(C3_TXT, "r"))) {
        perror(C3_TXT);
        exit(1);
    }

    long long sum = 0;
    int x, cnt = 0;
    struct timespec begin = get_time();

    { // code to be benchmarked
        while (fscanf(c3f, "%d", &x) && cnt++ < n) {
            sum += x;
            if  (feof(c3f)) break;
        }
    }

    // File does not contain at least n integers
    if (cnt < n) {
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C3_TXT, n, cnt);
        exit(2);
    }

    printf("[3] execution time: %lf\n", get_time_diff(begin, get_time()));
    fclose(c3f);

    return sum;
}