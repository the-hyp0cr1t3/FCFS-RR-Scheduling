#ifndef SCHEDULING
#define SCHEDULING
#include <semaphore.h>

#define SHM_CURRENT_SCHEDULED_FNAME "current_scheduled.shm"
// #define SHM_DONE_FNAME "done.shm"

static char* SHM_DONE[] = { "done0.shm", "done1.shm", "done2.shm" };

void *monitor(void *args);
void *worker(void *args);
void child_method(int process_id, sem_t *cpu_lock);

void rr_scheduler(char *shm_current_scheduled_block,int time_quantum,char *shm_done[]);
void fcfs_scheduler(char *shm_current_scheduled_block,char *shm_done[]);

#endif