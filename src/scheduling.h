#ifndef SCHEDULING
#define SCHEDULING
#include <semaphore.h>

#define SHM_CURRENT_RUNNING_FNAME "current_running.shm"
#define SHM_DONE_FNAME "done.shm"

void *monitor(void *args);
void *worker(void *args);
void child_method(int process_id, sem_t *cpu_lock);

#endif