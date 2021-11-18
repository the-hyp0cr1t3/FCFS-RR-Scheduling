#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"
#include "constants.h"
#include "process_info.h"
#include "child_process_threads.h"

void *monitor(void *args) {
    process_state *state = (process_state *)args;
    int turn_lock_val;

    while (!state->done) {
        /* Update the running status from the shared memory */
        state->current_scheduled = (int)*state->shm_current_scheduled;

        sem_getvalue(state->turn_lock, &turn_lock_val);

        // If the currently scheduled process is this process, and the value of the turn_lock semaphore is 0
        // (i.e. the worker thread is wating) then increment its value. This allows the worker thread to resume execution.
        if (state->current_scheduled == state->id && turn_lock_val == 0) {
            sem_post(state->turn_lock);
        }
        // If this is not the currently scheduled process and the value of the turn_lock sempahore is 1 (i.e. the worker thread is running)
        // then decrement its value. This indicates that the worker thread must wait on the turn_lock semaphore to resume execution.
        else if (state->current_scheduled != state->id && turn_lock_val == 1) {
            sem_wait(state->turn_lock);
        }

        usleep(MONITOR_POLL_FREQUENCY);  // sleep for 2us (polling frequency)
    }

    return state;
}

void *worker(void *args) {
    process_state *state = (process_state *)args;

    process_return *rtv = process_return_init(state->id);
    rtv->n = state->n;

    FILE *file = NULL;
    if (state->id != 0 && !(file = fopen(state->id == 1? C2_TXT : C3_TXT, "r"))) {
        perror(state->id == 1? C2_TXT : C3_TXT);
        exit(1);
    }

    int x, cnt = 0;
    long long int sum = 0;

    for (cnt = 0; cnt < state->n; cnt++) {
        int turn_val;
        struct timespec st = get_time();

        do {
            sem_getvalue(state->turn_lock, &turn_val);
        } while (!turn_val);
        sem_wait(state->cpu_lock);

        // Calculate the amount waited for this segment
        rtv->wts[rtv->wait_segments++] = get_time_diff(st, get_time());


        //  Critical Section Starts  ////////////////////////////////////////////////////////////

        if (state->id == 0) {
            x = rand() % MAX_VALUE + 1;
            sum += x;

        } else if (state->id == 1) {
            if (feof(file) || fscanf(file, "%d", &x) == 0) {
                fprintf(stderr, "%s: Expected %d, found %d integers.\n", C2_TXT, state->n, cnt);
                exit(2);
            }
            printf("%d\t", x);

        } else {
            if (feof(file) || fscanf(file, "%d", &x) == 0) {
                fprintf(stderr, "%s: Expected %d, found %d integers.\n", C3_TXT, state->n, cnt);
                exit(2);
            }
            sum += x;

        }

        //  Critical Section Ends  //////////////////////////////////////////////////////////////


        sem_post(state->cpu_lock);
    }

    if(file) fclose(file);
    fflush(stdout);

    // Calculate the turn around time
    rtv->tat = get_time_diff(rtv->start_time, get_time());
    rtv->result = sum;

    state->done = true;
    *state->shm_done = true;

    return rtv;
}
