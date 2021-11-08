#include "scheduling.h"

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process_state.h"
#include "shared_memory.h"

void *monitor(void *args) {
    process_state *state = (process_state *)args;

    // Update the running status from the shared memory
    while (!state->done) {
        memcpy(&state->current_running_proc, state->shm_block, sizeof(int));
    }
}

void *worker(void *args) {
    process_state *state = (process_state *)args;

    // Use pthread_cond_wait and pthread_cond_signal

    while (1) {
        printf("Child %d\n", state->id);
        // sleep(1);
    }


    // Write to the SHM_DONE to inform that the process is over.
    state->done = true;
}

void child_method(int process_id) {
    process_state *state;  // Initialized on the heap, to ensure that can be shared between threads.
    process_state_init(state, process_id, SHM_CURRENT_RUNNING_FILENAME, BLOCK_SIZE);

    pthread_t m_id, w_id;  // Monitor and Worker Thread IDs

    pthread_create(&m_id, NULL, monitor, (void *)state);
    pthread_create(&w_id, NULL, worker, (void *)state);

    pthread_join(w_id, NULL);
    pthread_join(m_id, NULL);

    process_state_destroy(state);
}
