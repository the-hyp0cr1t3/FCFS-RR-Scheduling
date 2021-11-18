#ifndef SCHEDULING
#define SCHEDULING

#include <semaphore.h>

/**
 * Create the turn_lock semaphore, monitor and worker thread;
 * Synchronize the threads, and wait for tasks to finish.
 * Log the details, and return the results.
 *
 * @param process_id: Process Number. Maybe 0, 1, 2 (Corresponding to C1, C2 and C3 from the problem statement)
 * @param cpu_lock: Semaphore on "CPU" Resource passed from the Parent Process.
 * @param num: Task Size for the Process at hand. (n1 if process_id is 0, n2 if process_id is 1, etc)
 * @return Result of Computation, if any. Undefined otherwise.
 */
long long int child_method(int process_id, sem_t *cpu_lock, int num);

// Simple Round Robin scheduler
void rr_scheduler(char *shm_current_scheduled_block, char *shm_done[]);

// Simple FCFS scheduler
void fcfs_scheduler(char *shm_current_scheduled_block, char *shm_done[]);

#endif