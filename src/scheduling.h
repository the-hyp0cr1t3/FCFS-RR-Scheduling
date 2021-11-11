#ifndef SCHEDULING
#define SCHEDULING

#include <semaphore.h>

void *monitor(void *args);
void *worker(void *args);
void child_method(int process_id, sem_t *cpu_lock);

void rr_scheduler(char *shm_current_scheduled_block, char *shm_done[], int time_quantum);
void fcfs_scheduler(char *shm_current_scheduled_block, char *shm_done[]);

#endif
