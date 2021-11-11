#include "process_state.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "shared_memory.h"

process_state* process_state_init(int process_id, sem_t* cpu_lock, int num) {
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
    state->result = 0;
    state->n = num;

    return state;
}

void process_state_destroy(process_state* state) {
    detach_memory_block(state->shm_current_scheduled);
    detach_memory_block(state->shm_done);

    sem_unlink(state->sem_turn_fname);

    free(state);
    state = NULL;
}

process_return* process_return_init(int process_id) {
    process_return* rtv = malloc(sizeof(process_return));

    rtv->id = process_id;
    timespec_get(&rtv->start_time, TIME_UTC);
    rtv->wait_segments = 0;
    rtv->wts = malloc((int)2e6 * sizeof(double));

    return rtv;
}

void serialize_process_return(process_return* rtv, char* filename) {
    double wt = 0;
    for (int i = 0; i < rtv->wait_segments; ++i) {
        wt += rtv->wts[i];
    }

    FILE* file = fopen(filename, "a");
    // process_id, n, start time, wait_iterations, total waiting time, turn around time
    fprintf(file,
            "%d,%d,%ld.%09ld,%d,%09lf,%09lf\n",
            rtv->id, rtv->n, rtv->start_time.tv_sec, rtv->start_time.tv_nsec, rtv->wait_segments, wt, rtv->tat);

    fclose(file);
}