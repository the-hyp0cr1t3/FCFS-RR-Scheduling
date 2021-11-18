#ifndef CHILD_PROCESS_THREADS
#define CHILD_PROCESS_THREADS

#include <semaphore.h>

/**
 * Created ONLY to be used as a thread routine.
 * The major components of this routine are the 'turn_lock' semaphore (referred to as 'turn'), 'done' status of the task, and the 'current_scheduled' shared memory block.
 * The scheduling algorithm writes the currently scheduled process to 'current_scheduled' shared memory block (shared amongst all processes).
 * This routine polls the status of this block at regular intervals to determine the process that should be running currently.
 * If the process id matches the currently scheduled process, the monitor thread signals the 'turn_lock' semaphore.
 * ___________________                                           ___________________
 * |                 |            ////////////////               |                 |
 * |                 |            /   Currently  /               |                 |
 * |    Scheduler    |  --------> /   Scheduled  /  <---------   |  Child Process  |
 * |                 |            /   Process    /               |                 |
 * |_________________|            ////////////////               |_________________|
 *
 * Intricacies:
 * - The polling is done at regular time intervals, say t, until the task assigned has finished, indicated by the done variable
 *   If t is too short, then the state update latency would be low but CPU usage would spike.
 *   If t is too long, then state update latency would be too high, and might miss the scheduler updates by a large margin.
 *
 * - The value of the 'turn' semaphore is always either 0 or 1.
 *    0 denotes it is not the currently scheduled process
 *    1 denotes it is the currently scheduled process
 *
 * @param args: process state declared on the heap; used for inter thread communitcation on the same process
 * @return return the final state of the process
 */
void *monitor(void *args);

/**
 * Workers created ONLY to be used as a thread routine.
 * The major components of this routine are the turn_lock and cpu_lock semaphores.
 *
 * We treat the CPU as a common resource, only one of which is available. This ensures that no two processes can do "effective work" at the same time.
 * i.e. the worker thread loops of no two processes can run simultaneously. This is ensured using the cpu_lock semaphore.
 * - the value of 'cpu_lock' is always either 0 or 1.
 *    0 denotes that the cpu is busy, and in use
 *    1 denotes that the cpu is free to be acquired by a process
 *
 * Before each iteration of the task begins, it must wait until the value of turn lock is 1, and then wait on the cpu_lock semaphore.
 * This is to ensure that it is actually the turn of this process, and that no other process is working right now.
 * At the end of each iteration, it must release the cpu_lock semaphore.
 * Consequently the waiting times, and turn around times are calculated in the worker thread only.
 *
 * @param args: process state declared on the heap; used for inter thread communitcation on the same process
 * @return final state of the process
*/
void *worker(void *args);

#endif