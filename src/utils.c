#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"

void check_file(char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "ERROR: File could not be created (%s)\n", filename);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void log_pids(pid_t parent, pid_t child[3], task ts) {
    FILE* log_file = fopen(LOG_FNAME, "a");

    // Log PIDs
    fprintf(log_file, "Process PIDS\n");
    fprintf(log_file, "M:\t%d\n", parent);
    fprintf(log_file, "C1:\t%d\tn:\t%d\n", child[0], ts.n1);
    fprintf(log_file, "C2:\t%d\tn:\t%d\n", child[1], ts.n2);
    fprintf(log_file, "C3:\t%d\tn:\t%d\n", child[2], ts.n3);

    fclose(log_file);
}

double get_time_diff(struct timespec st, struct timespec et) {
    return (double)(et.tv_sec - st.tv_sec) + ((double)(et.tv_nsec - st.tv_nsec) / 1000000000L);
}
