#ifndef SCHEDULING
#define SCHEDULING

#include <semaphore.h>

/**
 * Created ONLY to be used as a thread routine. 
 * The major components of this routine are the 'turn_lock' semaphore (referred to as 'turn'), 'done' status of the task, and the 'current_scheduled' shared memory block.
 * The scheduling algorithm writes the currently scheduled process to 'current_scheduled' shared memory block (shared amongst all processes). 
 * This routine polls the status of this block at regular intervals to determine the process that should be running currently.
 * If the process id matched the currently scheduled process, then the monitor thread signals the 'turn_lock' semaphore.
 * 
 * ___________________                                           ___________________
 * |                 |            ////////////////               |                 |
 * |                 |            /   Currently  /               |                 |
 * |    Scheduler    |  --------> /   Scheduled  /  <---------   |  Child Process  |
 * |                 |            /   Process    /               |                 |
 * |_________________|            ////////////////               |_________________|
 * 
 * Intricacies:
 * - The polling is done at regular time intervals, say t (until the task assigned has finished, indicated by the done variable.)
 *   If t is too short, then the state update latency would be low but CPU usage would spike.
 *   If t is too long, then state update latency would be too high, and might miss the scheduler updates by a large margin.
 * 
 * - The value of the 'turn' semaphore is always either 0 or 1. 
 * - We probably need some kind of mutex lock against reads/writes on the shared memory. Solve this problem later.
 * 
 * @param args: process state declared on the heap. Used for inter thread communitcation on the same process.
 * @return Return the final state of the process. Probably useless but what do I know.    
 */
void *monitor(void *args);

/**
 * Workers Created ONLY to be used as a thread routine.
 * The major components of this routine are the turn_lock and cpu_lock semaphores. 
 * 
 * We treat the CPU as a common resource, only one of which is available. This ensures that no two processes can do "effective work" at the same time. 
 * i.e. the worker thread loops of no two processes can run simultaneously.
 * 
 * Before each iteration of the task begins, it must wait on the turn_lock, and the cpu_lock semaphores. 
 * This is to ensure that it is actually the turn of this process, and that no other process is working right now. 
 * At the end of each iteration, it must release the cpu_lock semaphore.
 * 
 * Consequently the waiting times, and turn around times are calculated in the worker thread only. 
 * EVEN if some other process has been scheduled, due to a worker thread being inside the loop, might be working.
 * This causes an increase in the waiting time of the newly scheduled process and must be accounted for. 
 * Similary for the Turn Around times.
 * 
 * @param args: process state declared on the heap. Used for inter thread communitcation on the same process.
 * @return Final state of the process. Probably useless but what do I know.   
*/
void *worker0(void *args);
void *worker1(void *args);
void *worker2(void *args);

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

// Simple Round Robin Scheduler.
void rr_scheduler(char *shm_current_scheduled_block, char *shm_done[]);

// Simple FCFS Scheduler.
void fcfs_scheduler(char *shm_current_scheduled_block, char *shm_done[]);

#endif
