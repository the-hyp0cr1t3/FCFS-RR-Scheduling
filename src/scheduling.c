#include "scheduling.h"

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "process_state.h"
#include "shared_memory.h"

void *monitor(void *args) {
    process_state *state = (process_state *)args;
    int turn_lock_val;

    // Update the running status from the shared memory
    while (!state->done) {
        state->current_running_proc = *state->shm_block;
        printf("LOG [%d]: Current Running Process: %d\n", state->id, state->current_running_proc);
        sem_getvalue(state->turn_lock, &turn_lock_val);

        if (state->current_running_proc == state->id && turn_lock_val == 0) {
            // If it is our turn, set the value of the turn semaphore to 1
            sem_post(state->turn_lock);
        } else if (state->current_running_proc != state->id && turn_lock_val == 1) {
            // If it is not our turn, set the value of the turn variable to 0
            sem_wait(state->turn_lock);
        }

        // usleep(2000);  //sleep for 2000 us or 2ms
        sleep(2);  //sleep for 2000 ms or 2s
    }

    return state;
}

void *worker(void *args) {
    process_state *state = (process_state *)args;

    // Use pthread_cond_wait and pthread_cond_signal

    while (1) {
        sem_wait(state->cpu_lock);
        printf("Child %d\n", state->id);
        sleep(1);
        sem_post(state->cpu_lock);
    }

    // Write to the SHM_DONE to inform that the process is over.
    state->done = true;
}

void child_method(int process_id, sem_t *cpu_lock) {  // Move cpu_lock to be a process local variable?
    // Initialized on the heap, to ensure that can be shared between threads.
    process_state *state = process_state_init(process_id, SHM_CURRENT_RUNNING_FNAME, BLOCK_SIZE, cpu_lock);

    pthread_t m_id, w_id;  // Monitor and Worker Thread IDs

    pthread_create(&m_id, NULL, monitor, (void *)state);
    pthread_create(&w_id, NULL, worker, (void *)state);

    pthread_join(w_id, NULL);
    pthread_join(m_id, NULL);

    process_state_destroy(state);
}
