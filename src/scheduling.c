#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"
#include "utils.h"
#include "process_info.h"
#include "child_process_threads.h"

long long int child_method(int process_id, sem_t *cpu_lock, int num) {
    /* Initialized on the heap, to ensure that can be shared between threads. */
    process_state *state = process_state_init(process_id, cpu_lock, num);
    process_return *rtv;

    pthread_t m_id, w_id; /* Monitor and Worker thread IDs */
    pthread_create(&m_id, NULL, monitor, (void *)state);
    pthread_create(&w_id, NULL, worker, (void *)state);

    pthread_join(w_id, (void **)&rtv);
    pthread_join(m_id, NULL);

    serialize_process_return(rtv);

    long long int res = rtv->result;

    process_state_destroy(state);
    process_return_destroy(rtv);

    return res;
}

void rr_scheduler(char *shm_current_scheduled_block, char *shm_done[]) {

    while (true) {
        int current_scheduled = *shm_current_scheduled_block;
        int new_schedule_offset = 0;

        for (new_schedule_offset = 1; new_schedule_offset <= 3; new_schedule_offset++) {
            if (*shm_done[(current_scheduled + new_schedule_offset) % 3] == 0) break;
        }

        // If the loop reaches the end, without breaking it means that all tasks have finished successfully,
        // and the done shared memory block of each process has 1 in it. Congratulations!
        if (new_schedule_offset > 3) break;

        // Once a process to be scheduled has been found, schedule it by writing to the shared memory block
        current_scheduled = (current_scheduled + new_schedule_offset) % 3;
        *shm_current_scheduled_block = current_scheduled;

        struct timespec current_time = get_time();
        double fmted_time = current_time.tv_sec + (current_time.tv_nsec / 1e9);
        printf("\n[%lf] scheduled: %d\n", fmted_time, current_scheduled);
        fflush(stdout);

        usleep(get_time_quantum());  // sleep for time quantum before scheduling the next proccess
    }
}

void fcfs_scheduler(char *shm_current_scheduled_block, char *shm_done[]) {
    // Iterate over each process, schedule next only after the current has finished execution.
    // Assumption: Ci arrived before Cj if i < j
    for (int i = 0; i < 3; i++) {
        *shm_current_scheduled_block = i;

        struct timespec current_time = get_time();
        double fmted_time = current_time.tv_sec + (current_time.tv_nsec / 1e9);
        printf("\n[%lf] scheduled: %d\n", fmted_time, *shm_current_scheduled_block);
        fflush(stdout);

        while (*shm_done[i] == 0) {
            usleep(FCFS_MASTER_POLL_FREQUENCY);  // check if the process is completed or not
        }
    }
}