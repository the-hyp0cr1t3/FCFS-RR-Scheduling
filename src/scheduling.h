#ifndef SCHEDULING
#define SCHEDULING
#include <semaphore.h>

#define SHM_CURRENT_SCHEDULED_FNAME "current_scheduled.shm"
#define SHM_DONE_FNAME "done.shm"

void *monitor(void *args);
void *worker(void *args);
void child_method(int process_id, sem_t *cpu_lock);

void rr_scheduler(char *shm_current_scheduled_block);
void fcfs_scheduler(char *shm_current_scheduled_block);

#endif