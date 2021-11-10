#include "scheduling.h"

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "process_state.h"
#include "shared_memory.h"

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
 * @return state: return the final state of the process. Probably useless but what do I know.    
 */
void *monitor(void *args) {
    process_state *state = (process_state *)args;
    int turn_lock_val;

    while (!state->done) {
        /* Update the running status from the shared memory */
        state->current_scheduled = *state->shm_current_scheduled;

        // printf("LOG [%d]: Current Running Process: %d\n", state->id, state->current_running_proc);
        sem_getvalue(state->turn_lock, &turn_lock_val);

        /*
            If the currently scheduled process is this process, and the value of the turn_lock semaphore is 0 (i.e. the worker thread is wating)
            then increment its value. This allows the worker thread to resume execution.
            If this is not the currently scheduled process and the value of the turn_lock sempahore is 1 (i.e. the worker thread is running)
            then decrement its value. This indicates that the worker thread must wait on the turn_lock semaphore to resume execution.
        */

        if (state->current_scheduled == state->id && turn_lock_val == 0) {
            sem_post(state->turn_lock);
        } else if (state->current_scheduled != state->id && turn_lock_val == 1) {
            sem_wait(state->turn_lock);
        }

        usleep(2000);  //sleep for 2000 us or 2ms
    }

    return state;
}

/**
 * Created ONLY to be used as a thread routine.It is a sample routine, and the actual routines must be based on this architecture/model.
 * The major components of this routine are the turn_lock and cpu_lock semaphores. 
 * 
 * We treat the CPU as a common resource, only one of which is available. This ensures that no two processes can do "effective work" at the same time. 
 * i.e. the worker thread loops of no two processes can run simultaneously.
 * 
 * Before each iteration of the task begins, it must wait on the turn_lock, and the cpu_lock semaphores. 
 * This is to ensure that it is actually the turn of this process, and that no other process is working right now. 
 * At the end of each iteration, it must release the cpu_lock semaphore.
 * 
 * @param args: process state declared on the heap. Used for inter thread communitcation on the same process.
 * @return state: return the final state of the process. Probably useless but what do I know.   
*/

void *worker(void *args) {
    process_state *state = (process_state *)args;

    for (int iters = 0; iters < 6 * (state->id + 1); iters++) {
        sem_wait(state->turn_lock);
        sem_wait(state->cpu_lock);

        //  Critical Section Starts
        printf("%d: Child %d\n", iters, state->id);
        sleep(1);
        // Critical Section Ends

        sem_post(state->cpu_lock);
    }

    // Write to the SHM_DONE to inform that the process is over.
    state->done = true;
    *state->shm_done = true;


}

/**
 * Can't rant. This is too straightforward.
 */
void child_method(int process_id, sem_t *cpu_lock) {  // Move cpu_lock to be a process local variable?
    /* Initialized on the heap, to ensure that can be shared between threads. */
    process_state *state = process_state_init(process_id, SHM_CURRENT_SCHEDULED_FNAME, SHM_DONE[process_id], BLOCK_SIZE, cpu_lock);

    pthread_t m_id, w_id; /* Monitor and Worker Thread IDs */

    pthread_create(&m_id, NULL, monitor, (void *)state);
    pthread_create(&w_id, NULL, worker, (void *)state);

    pthread_join(w_id, NULL);
    pthread_join(m_id, NULL);

    process_state_destroy(state);
}

void rr_scheduler(char *shm_current_scheduled_block, int time_quantum,char *shm_done[]) {
    while (true) {
        int cur = *shm_current_scheduled_block, add;

        for(add = 1; add <= 3; add++){
            if(!*shm_done[(cur + add) % 3]) break;
        }

        if(add > 3) break;

        *shm_current_scheduled_block = (cur + add) % 3;
        // printf("LOG [M]: shm_current_scheduled_block = %d\n", *shm_current_scheduled_block);

        sleep(time_quantum);  // sleep for time quantum
    }
}

void fcfs_scheduler(char *shm_current_scheduled_block,char *shm_done[]) {
        for(int i=0;i<3;i++){
            *shm_current_scheduled_block=i;
            //sleep(large time)
        }
}