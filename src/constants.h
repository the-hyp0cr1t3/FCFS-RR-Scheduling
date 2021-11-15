#ifndef CONSTANTS
#define CONSTANTS

#define SEM_CPU_FNAME "/cpu"
#define SHM_BLOCK_SIZE 4096
#define SHM_CURRENT_SCHEDULED_FNAME "current_scheduled.shm"

#define PIPE_READ 0
#define PIPE_WRITE 1

#define DEFAULT_TIME_QUANTUM (int)(1e3)  // 1x10^3 us = 1ms
#define NUM (int)(1e6)                   // 1 million

#define C2_TXT "c2.txt"
#define C3_TXT "c3.txt"

#define STATS_FNAME "stats.csv"
#define LOG_FNAME "exec.log"

static const char *SHM_DONE_FNAMES[] = {"done_0.shm", "done_1.shm", "done_2.shm"};

#define STATS_FNAME "stats.csv"
#endif