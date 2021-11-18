#ifndef PARSE_ARGS
#define PARSE_ARGS

typedef struct arg_data {
    int n1, n2, n3;                 // workloads
    int time_quantum;               // in microseconds
    int scheduling_algorithm;       // FCFS or RR
} arg_data;

/**
* Parses the command line arguments.
* Returns a struct arg_data.
*
* Following are the acceptable ways to pass commandline args ('bin/main' is the binary executable)
* 1. To run with default arguments run:
*     ./bin/main                  Runs fcfs scheduling algorithm with default values for workloads n1 = n2 = n3 = 1e6
*
* 2. To run with First Come First Serve (FCFS) scheduling algorithm:
*     ./bin/main fcfs             Runs with default values for workloads n1 = n2 = n3 = 1e6
*     ./bin/main fcfs n1 n2 n3    Runs with given values of workloads
*
* 3. To run with Round Robin (RR) scheduling algorithm:
*     ./bin/main rr               Runs with default values for workloads n1 = n2 = n3 = 1e6 and time quantum tq = 1e3 us (micro-seconds)
*     ./bin/main rr tq            Runs with time quantum tq (us) and default values of workloads n1 = n2 = n3 = 1e6
*     ./bin/main rr n1 n2 n3      Runs with default time quantum tq = 1e3 us (micro-seconds) and given workloads
*     ./bin/main rr tq n1 n2 n3   Runs with time quantum tq (us) and given workloads
*
* @param argc: number of command line arguments
* @param argv: command line arguments as char* buffer
* @return Returns parsed args
*/
arg_data parse_args(int argc, char *argv[]);

#endif