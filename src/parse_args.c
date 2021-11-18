#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "parse_args.h"

arg_data parse_args(int argc, char *argv[]) {
    arg_data args = {
        .n1 = DEFAULT_N,
        .n2 = DEFAULT_N,
        .n3 = DEFAULT_N,
        .time_quantum = __INT_MAX__,    // time quantum for FCFS is as good as infinity
        .scheduling_algorithm = FCFS    // FCFS by default
    };

    // set scheduling algorithm if specified
    if(argc > 1) {
        char buff[5];
        strcpy(buff, argv[1]);
        if (strcmp(buff, "rr") == 0) {
            args.scheduling_algorithm = RR;
            args.time_quantum = DEFAULT_TIME_QUANTUM;
        } else if (strcmp(buff, "fcfs") != 0) {
            fprintf(stderr, "Invalid scheduling algorithm \"%s\". Use \"fcfs\" or \"rr\".\n", buff);
            exit(EXIT_FAILURE);
        }
    }

    switch(argc) {
        case 1:
        /*   ./bin/main
        */

        case 2:
        /*   ./bin/main fcfs
             ./bin/main rr
        */
            break;

        case 3:
        /*   ./bin/main rr tq
        */
            if (args.scheduling_algorithm != RR) {
                fprintf(stderr, "Must set time quantum for RR only. Use ./bin/main rr tq\n");
                exit(EXIT_FAILURE);
            }

            args.time_quantum = atoi(argv[2]);
            break;

        case 5:
        /*   ./bin/main fcfs n1 n2 n3
             ./bin/main rr   n1 n2 n3
        */
            args.n1 = atoi(argv[2]);
            args.n2 = atoi(argv[3]);
            args.n3 = atoi(argv[4]);
            break;

        case 6:
        /*   ./bin/main rr tq n1 n2 n3
        */
            if (args.scheduling_algorithm != RR) {
                fprintf(stderr, "Must set time quantum for RR only. Use ./bin/main rr tq n1 n2 n3\n");
                exit(EXIT_FAILURE);
            }

            args.time_quantum = atoi(argv[2]);
            args.n1 = atoi(argv[3]);
            args.n2 = atoi(argv[4]);
            args.n3 = atoi(argv[5]);
            break;

        default:
            fprintf(stderr, "Invalid argument list.\n");
            exit(EXIT_FAILURE);
            break;
    }


    FILE *log_file = fopen(LOG_FNAME, "a");

    // Basic Logs
    fprintf(log_file, "Scheduling algorithm: %s\n", args.scheduling_algorithm == RR? "RR" : "FCFS");
    fprintf(log_file, "Time quantum: %d\n", args.time_quantum);

    fclose(log_file);

    time_quantum = args.time_quantum;
    scheduling_algorithm = args.scheduling_algorithm;

    return args;
}
