#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_VALUE 1000000   // generate values in the range [1, 1e6]
#define DEFAULT_N 1000000   // default size of file 1e6
#define C2_TXT "c2.txt"
#define C3_TXT "c3.txt"

// Utility function to generate text files for child processes
void make_txt(char *fname, int n) {
    FILE *file;
    if (!(file = fopen(fname, "w"))) {
        perror("Could not open file");
        exit(1);
    }
 
    for (int i = 0; i < n; ++i)
        fprintf(file, "%d\n", rand() % MAX_VALUE + 1);
 
    fclose(file);
}
 
int main(int argc, char* argv[]) {
    srand(time(NULL));
    int n2, n3;
    n2 = n3 = DEFAULT_N;
 
    // accepts n2 and n3 as command line arguements (else uses defaults)
    if (argc >= 2)
        n2 = atoi(argv[1]);

    if (argc >= 3)
        n3 = atoi(argv[2]);

    make_txt(C2_TXT, n2);
    make_txt(C3_TXT, n3);

    printf("Created c2.txt c3.txt\n");
}
