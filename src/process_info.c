#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "constants.h"
#include "shared_memory.h"
#include "process_info.h"

const char *get_scheduling_algorithm() {
    return scheduling_algorithm == FCFS? "fcfs" : "rr";
}

int get_time_quantum() {
    return time_quantum;
}

process_state* process_state_init(int process_id, sem_t* cpu_lock, int num) {
    process_state* state = malloc(sizeof(process_state)); /* Initialized on the heap, to ensure that can be shared between threads. */
    state->id = process_id;

    state->done = false;

    state->current_scheduled = -1;
    state->cpu_lock = cpu_lock;
    snprintf(state->sem_turn_fname, 16, "/turn_%d", state->id);

    sem_unlink(state->sem_turn_fname);
    state->turn_lock = sem_open(state->sem_turn_fname, O_CREAT, 0644, 0);

    // Initialize Shared Memory Block
    state->shm_current_scheduled = attach_memory_block(SHM_CURRENT_SCHEDULED_FNAME, SHM_BLOCK_SIZE);
    if (state->shm_current_scheduled == NULL) {
        fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
        exit(EXIT_FAILURE);
    }

    state->shm_done = attach_memory_block(SHM_DONE_FNAMES[state->id], SHM_BLOCK_SIZE);
    if (state->shm_done == NULL) {
        fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_DONE_FNAMES[state->id]);
        exit(EXIT_FAILURE);
    }
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
    rtv->start_time = get_time();
    rtv->wait_segments = 0;
    rtv->wts = malloc((int)2e6 * sizeof(double));

    return rtv;
}

void process_return_destroy(process_return* rtv) {
    free(rtv);
    rtv = NULL;
}

void serialize_process_return(process_return* rtv) {
    char formatted_time[100];
    strftime(formatted_time, sizeof formatted_time, "%D %T", gmtime(&rtv->start_time.tv_sec));

    double wt = 0;
    for (int i = 0; i < rtv->wait_segments; ++i) {
        wt += rtv->wts[i];
    }

    FILE* log_file = fopen(LOG_FNAME, "a");
    fprintf(log_file, "PROCESS: %d\n", rtv->id);
    fprintf(log_file, "[%d] Start Time: %s.%09ld UTC\n", rtv->id, formatted_time, rtv->start_time.tv_nsec);
    fprintf(log_file, "[%d] Number of wait segments: %d\n", rtv->id, rtv->wait_segments);
    fprintf(log_file, "[%d] Total Waiting Time for this Process: %09lf\n", rtv->id, wt);
    fprintf(log_file, "[%d] Turn-Around Time: %09lf\n", rtv->id, rtv->tat);
    fclose(log_file);

    FILE* stats_file = fopen(STATS_FNAME, "a");
    // scheduling_algo, process_id, n, time_quantum, start time, wait_iterations, total waiting time, turn around time
    fprintf(stats_file,
            "%s,%d,%d,%d,%ld.%09ld,%d,%09lf,%09lf\n",
            get_scheduling_algorithm(),
            rtv->id, rtv->n, get_time_quantum(),
            rtv->start_time.tv_sec, rtv->start_time.tv_nsec, rtv->wait_segments, wt, rtv->tat);

    fclose(stats_file);
}