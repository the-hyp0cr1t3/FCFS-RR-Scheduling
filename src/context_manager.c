#include "context_manager.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"

// Scheduling Algorithm and Time Quantum can be set ONLY using arg_parse function, and hence can not be tampered with.
// This is a security feature, and prevents accidental modification of these variables.
// The singleton pattern also prevents the need for any kind of argument drilling, and long chained argument passing.
static char scheduling_algorithm[8];
static int time_quantum;

task argparse(int argc, char *argv[]) {
    task ts = {.n1 = (int)(1e6), .n2 = (int)(1e6), .n3 = (int)(1e6)};

    // Parese Arguments
    if (argc == 1) {
        strcpy(scheduling_algorithm, "fcfs");
        time_quantum = __INT_MAX__;
    } else {
        strcpy(scheduling_algorithm, argv[1]);

        if (argc == 2 && strcmp(scheduling_algorithm, "rr") == 0) {
            time_quantum = DEFAULT_TIME_QUANTUM;
        }

        if (argc == 3) {  // bin algo tq
            time_quantum = atoi(argv[2]);
        }

        if (argc == 5) {  // bin algo n1 n2 n3; algo maybe be fcfs or rr
            ts.n1 = atoi(argv[2]);
            ts.n2 = atoi(argv[3]);
            ts.n3 = atoi(argv[4]);
        }

        if (argc == 6) {  // bin rr tq n1 n2 n3
            if (strcmp(scheduling_algorithm, "rr")) {
                fprintf(stderr, "ERROR: Argument Parsing failed due to Invalid Argument List. Use one of the specified formats only\n");
                exit(EXIT_FAILURE);
            }

            time_quantum = atoi(argv[2]);
            ts.n1 = atoi(argv[3]);
            ts.n2 = atoi(argv[4]);
            ts.n3 = atoi(argv[5]);
        }
    }

    FILE *log_file = fopen(LOG_FNAME, "a");

    // Basic Logs
    fprintf(log_file, "Scheduling Algorithm: %s\n", scheduling_algorithm);
    fprintf(log_file, "Time Quntum: %d\n", time_quantum);
    fprintf(log_file, "Batch Size: %d\n", BATCH_SIZE);

    fclose(log_file);

    return ts;
}

char *get_scheduling_algorithm() {
    return scheduling_algorithm;
}

int get_time_quantum() {
    return time_quantum;
}