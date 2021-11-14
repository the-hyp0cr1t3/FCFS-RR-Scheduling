#ifndef SCHEDULING
#define SCHEDULING

#include <semaphore.h>

void *monitor(void *args);
void *worker0(void *args);
void *worker1(void *args);
void *worker2(void *args);

long long int child_method(int process_id, sem_t *cpu_lock, int num);

void rr_scheduler(char *shm_current_scheduled_block, char *shm_done[]);
void fcfs_scheduler(char *shm_current_scheduled_block, char *shm_done[]);

#endif
