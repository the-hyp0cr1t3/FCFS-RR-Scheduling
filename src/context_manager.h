#ifndef STATE_MANAGER
#define STATE_MANAGER

#include <semaphore.h>

typedef struct task {
    int n1, n2, n3;  // A task is defined by the size of the variables n1, n2, n3
} task;

/**
* Parses the Command Line Arguments and set the scheduling_algorithm and time_quantum variables.
* Returns a task, after parsing the n1, n2 and n3 values.
* Parsing is consisten only if  a few acceptable ways to call the program are used.
* Any other invocation, or argument list results in undefined behaviour
* 1. (binary) - Called with all default parameters
* 2. (binary) algorithm - Called with scheduling algorithm specified. Must be fcfs or rr. All other parameters are used with default values
* 3. (binary) algorithm n1 n2 n3 - Called with algorithm and task sizes specified. Default time quantum used.
* 4. (binary) algorithm tq n1 n2 n3 - Algorithm must be rr. All Parameters specified.
* 
* @param argc: Number of command line arguments
* @param argv: Command Line Arguments as char* buffers
* @return ts: Returns parsed task sizes
*/
task argparse(int argc, char *argv[]);

// Returns the scheduling algorithm set after parsing.
// Return value is undefined if called before argparse.
char *get_scheduling_algorithm();

// Returns the time quantum set after parsing.
// Return value is undefined if called before argparse.
int get_time_quantum();

#endif