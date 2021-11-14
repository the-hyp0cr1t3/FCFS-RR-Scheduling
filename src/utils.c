#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

void check_file(char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "ERROR: File could not be created (%s)\n", filename);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

double get_time_diff(struct timespec st, struct timespec et) {
    return (double)(et.tv_sec - st.tv_sec) + ((double)(et.tv_nsec - st.tv_nsec) / 1000000000L);
}
