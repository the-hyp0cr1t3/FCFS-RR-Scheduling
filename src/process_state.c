#include "process_state.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "shared_memory.h"

process_state* process_state_init(int process_id, sem_t* cpu_lock) {
    process_state* state = malloc(sizeof(process_state)); /* Initialized on the heap, to ensure that can be shared between threads. */
    state->id = process_id;

    state->done = false;

    state->current_scheduled = -1;
    state->cpu_lock = cpu_lock;
    snprintf(state->sem_turn_fname, 16, "/turn_%d", state->id);

    sem_unlink(state->sem_turn_fname);
    state->turn_lock = sem_open(state->sem_turn_fname, O_CREAT, 0644, 0);

    /* Initialize Shared Memory Block */
    state->shm_current_scheduled = attach_memory_block(SHM_CURRENT_SCHEDULED_FNAME, SHM_BLOCK_SIZE);
    if (state->shm_current_scheduled == NULL) {
        fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
        exit(EXIT_FAILURE);
    }

    state->shm_done = attach_memory_block(SHM_DONE[state->id], SHM_BLOCK_SIZE);
    if (state->shm_done == NULL) {
        fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_DONE[state->id]);
        exit(EXIT_FAILURE);
    }

    return state;
}

void process_state_destroy(process_state* state) {
    detach_memory_block(state->shm_current_scheduled);
    detach_memory_block(state->shm_done);

    sem_unlink(state->sem_turn_fname);

    free(state);
    state = NULL;
}