#include "process_state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared_memory.h"

void process_state_init(process_state* state, int process_id, char* shm_filename, int shm_size) {
    state = malloc(sizeof(process_state));  // Initialized on the heap, to ensure that can be shared between threads.
    state->id = process_id;
    state->done = false;
    state->current_running_proc = -1;

    // Initialize Shared Memory Block
    state->shm_block = attach_memory_block(shm_filename, shm_size);
    if (state->shm_block == NULL) {
        fprintf(stderr, "ERROR: Could not get block: %s\n", shm_filename);
        exit(EXIT_FAILURE);
    }

    if (pthread_cond_init(&state->cond, NULL) != 0) {
        perror("ERROR: pthread_cond_init failed");
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&state->lock, NULL) != 0) {
        perror("ERROR: pthread_mutex_init failed");
        exit(EXIT_FAILURE);
    }
}

void process_state_destroy(process_state* state) {
    if (pthread_mutex_destroy(&state->lock) != 0) {
        perror("ERROR: pthread_mutex_destroy failed");
        exit(EXIT_FAILURE);
    }

    if (pthread_cond_destroy(&state->cond) != 0) {
        perror("ERROR: pthread_cond_destroy failed");
        exit(EXIT_FAILURE);
    }

    detach_memory_block(state->shm_block);

    free(state);
    state = NULL;
}