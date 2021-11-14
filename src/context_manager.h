#ifndef STATE_MANAGER
#define STATE_MANAGER

#include <semaphore.h>

typedef struct task {
    int n1, n2, n3;
} task;

task argparse(int argc, char *argv[]);

char *get_scheduling_algorithm();
int get_time_quantum();

#endif