#ifndef CONSTANTS
#define CONSTANTS

#define SEM_CPU_FNAME               "/cpu"
#define SHM_BLOCK_SIZE               4096
#define SHM_CURRENT_SCHEDULED_FNAME "current_scheduled.shm"

#define FCFS        0
#define RR          1
#define PIPE_READ   0
#define PIPE_WRITE  1

#define DEFAULT_TIME_QUANTUM   1000       // default time quantum 10^3 us = 1ms
#define MAX_VALUE              1000000    // generate values in the range [1, 10^6]
#define DEFAULT_N              1000       // default workload size 10^3

#define MONITOR_POLL_FREQUENCY      2     // frequency (in microseconds) at which monitor thread polls the currently scheduled shared memory block
#define FCFS_MASTER_POLL_FREQUENCY  2     // frequency (in microseconds) at which master process polls the done shared memory block

#define C2_TXT       "c2.txt"
#define C3_TXT       "c3.txt"
#define LOG_FNAME    "exec.log"
#define STATS_FNAME  "stats.csv"
#define C2_DONE_MSG  "Done printing"

static const char *SHM_DONE_FNAMES[] = { "done_0.shm", "done_1.shm", "done_2.shm" };

// scheduling_algorithm and time_quantum can be set ONLY in parse_args, and hence can not be tampered with.
// This is a security feature, and prevents accidental modification of these variables.
// The singleton pattern also prevents the need for any kind of argument drilling, and long chained argument passing.
static int scheduling_algorithm, time_quantum;

#endif