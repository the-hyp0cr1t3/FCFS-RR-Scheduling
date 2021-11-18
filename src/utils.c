#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "utils.h"

// create a file if it does not exist
void create_if_not_exist(const char* filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "ERROR: File could not be created (%s)\n", filename);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

// write the pids of the child processes to the log file
void log_pids(pid_t parent, pid_t child[3], arg_data args) {
    FILE* log_file = fopen(LOG_FNAME, "a");

    // Log PIDs
    fprintf(log_file, "Process PIDS\n");
    fprintf(log_file, "M:\t%d\n", parent);
    fprintf(log_file, "C1:\t%d\tn:\t%d\n", child[0], args.n1);
    fprintf(log_file, "C2:\t%d\tn:\t%d\n", child[1], args.n2);
    fprintf(log_file, "C3:\t%d\tn:\t%d\n", child[2], args.n3);

    fclose(log_file);
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