#include "scheduling.h"

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"
#include "context_manager.h"
#include "process_state.h"
#include "shared_memory.h"
#include "utils.h"

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
        state->current_scheduled = (int)*state->shm_current_scheduled;

        // printf("LOG [%d]: Current Running Process: %d\n", state->id, state->current_running_proc);
        sem_getvalue(state->turn_lock, &turn_lock_val);

        /*
            If the currently scheduled process is this process, and the value of the turn_lock semaphore is 0 (i.e. the worker thread is wating)
            then increment its value. This allows the worker thread to resume execution.
            If this is not the currently scheduled process and the value of the turn_lock sempahore is 1 (i.e. the worker thread is running)
            then decrement its value. This indicates that the worker thread must wait on the turn_lock semaphore to resume execution.
        */

        if (state->current_scheduled != state->id && turn_lock_val == 1) {
            sem_wait(state->turn_lock);
            sem_post(state->cpu_lock);

        }

        else if (state->current_scheduled == state->id && turn_lock_val == 0) {
            sem_wait(state->cpu_lock);
            sem_post(state->turn_lock);
        }

        usleep(2000);  //sleep for 2000 us or 2ms
    }

    int cpu_lock_val;
    sem_getvalue(state->cpu_lock, &cpu_lock_val);
    if (cpu_lock_val == 0) {
        sem_post(state->cpu_lock);
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
void *worker0(void *args) {
    process_state *state = (process_state *)args;

    process_return *rtv = process_return_init(state->id);
    rtv->n = state->n;

    //printf("[%d] Started Execution at: ", state->id);

    struct timespec st, et;

    int x, cnt = 0;
    long long int sum = 0;

    for (cnt = 0; cnt < state->n;) {
        if (timespec_get(&st, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        int turn_val;

        do {
            sem_getvalue(state->turn_lock, &turn_val);
        } while (!turn_val);
        //  sem_wait(state->cpu_lock);

        if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        int batched;
        for (batched = 0; batched + cnt < state->n && batched < BATCH_SIZE; ++batched) {
            //  Critical Section Starts
            x = rand() % NUM + 1;
            sum += x;
            // Critical Section Ends
        }

        cnt += batched;

        // Critical Section Ends
        //  sem_post(state->cpu_lock);

        // Calculate the amount waited for this segment
        rtv->wts[rtv->wait_segments++] = get_time_diff(st, et);
    }

    if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    // Calculate the turn around time
    rtv->tat = get_time_diff(rtv->start_time, et);

    // Write to the SHM_DONE to inform that the process is over.
    state->done = true;
    state->result = sum;
    *state->shm_done = true;

    return rtv;
}

void *worker1(void *args) {
    process_state *state = (process_state *)args;
    process_return *rtv = process_return_init(state->id);
    rtv->n = state->n;

    //printf("[%d] Started Execution at: ", state->id);

    struct timespec st, et;

    FILE *c2f;
    if (!(c2f = fopen(C2_TXT, "r"))) {
        perror(C2_TXT);
        exit(1);
    }
    int x, cnt = 0;
    for (cnt = 0; cnt < state->n;) {
        if (timespec_get(&st, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        int turn_val;

        do {
            sem_getvalue(state->turn_lock, &turn_val);
        } while (!turn_val);
        //  sem_wait(state->cpu_lock);

        // The wait is over!
        if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        //  Critical Section Starts
        int batched;
        for (batched = 0; batched + cnt < state->n && batched < BATCH_SIZE; ++batched) {
            if (fscanf(c2f, "%d", &x) == 0) {
                break;
            }

            printf("%d\t", x);
        }

        cnt += batched;
        // Critical Section Ends
        //  sem_post(state->cpu_lock);

        // Calculate the amount waited for this segment
        rtv->wts[rtv->wait_segments++] = get_time_diff(st, et);

        // Critical Section Ends
        if (feof(c2f)) break;
    }

    if (cnt < state->n) {
        // Prints the numbers in the file even if cnt < n. Is it an issue?
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C2_TXT, state->n, cnt);
        exit(2);
    }

    // Write to the SHM_DONE to inform that the process is over.
    fclose(c2f);
    if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    fflush(stdout);

    // Calculate the turn around time
    rtv->tat = get_time_diff(rtv->start_time, et);
    state->done = true;
    *state->shm_done = true;
    return rtv;
}

void *worker2(void *args) {
    process_state *state = (process_state *)args;
    process_return *rtv = process_return_init(state->id);
    rtv->n = state->n;

    //printf("[%d] Started Execution at: ", state->id);

    struct timespec st, et;

    FILE *c3f;
    if (!(c3f = fopen(C3_TXT, "r"))) {
        perror(C2_TXT);
        exit(1);
    }
    int x, cnt = 0;
    long long int sum = 0;
    for (cnt = 0; cnt < state->n;) {
        // Calculate the amount waited for this segment
        if (timespec_get(&st, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        int turn_val;

        do {
            sem_getvalue(state->turn_lock, &turn_val);
        } while (!turn_val);
        //  sem_wait(state->cpu_lock);

        // The wait is over!
        if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        int batched;
        for (batched = 0; batched + cnt < state->n && batched < BATCH_SIZE; ++batched) {
            //  Critical Section Starts
            if (fscanf(c3f, "%d", &x) == 0) {
                break;
            }

            sum += x;
            // Critical Section Ends
        }

        cnt += batched;

        // Critical Section Ends
        //  sem_post(state->cpu_lock);
        rtv->wts[rtv->wait_segments++] = get_time_diff(st, et);

        if (feof(c3f)) break;
    }
    if (cnt < state->n) {
        // Prints the numbers in the file even if cnt < n. Is it an issue?
        fprintf(stderr, "%s: Expected %d, found %d integers.\n", C3_TXT, state->n, cnt);
        exit(2);
    }
    // Write to the SHM_DONE to inform that the process is over.
    fclose(c3f);
    if (timespec_get(&et, TIME_UTC) != TIME_UTC) {
        fprintf(stderr, "ERROR: call to timespec_get failed \n");
        exit(EXIT_FAILURE);
    }

    // Calculate the turn around time
    rtv->tat = get_time_diff(rtv->start_time, et);
    state->done = true;
    state->result = sum;
    *state->shm_done = true;
    return rtv;
}

/**
 * Can't rant. This is too straightforward.
 */
long long int child_method(int process_id, sem_t *cpu_lock, int num) {  // Move cpu_lock to be a process local variable?
    /* Initialized on the heap, to ensure that can be shared between threads. */
    process_state *state = process_state_init(process_id, cpu_lock, num);
    process_return *rtv;

    pthread_t m_id, w_id; /* Monitor and Worker Thread IDs */
    pthread_create(&m_id, NULL, monitor, (void *)state);
    if (process_id == 0)
        pthread_create(&w_id, NULL, worker0, (void *)state);
    else if (process_id == 1)
        pthread_create(&w_id, NULL, worker1, (void *)state);
    else
        pthread_create(&w_id, NULL, worker2, (void *)state);

    pthread_join(w_id, (void **)&rtv);
    pthread_join(m_id, NULL);

    char buff[100];
    strftime(buff, sizeof buff, "%D %T", gmtime(&rtv->start_time.tv_sec));

    double wt = 0;
    for (int i = 0; i < rtv->wait_segments; ++i) {
        wt += rtv->wts[i];
    }

    FILE *log_file = fopen(LOG_FNAME, "a");
    fprintf(log_file, "PROCESS: %d\n", rtv->id);
    fprintf(log_file, "[%d] Start Time: %s.%09ld UTC\n", rtv->id, buff, rtv->start_time.tv_nsec);
    fprintf(log_file, "[%d] Number of wait segments: %d\n", rtv->id, rtv->wait_segments);
    fprintf(log_file, "[%d] Total Waiting Time for this Process: %09lf\n", rtv->id, wt);
    fprintf(log_file, "[%d] Turn-Around Time: %09lf\n", rtv->id, rtv->tat);
    fclose(log_file);

    serialize_process_return(rtv);

    long long int res = state->result;

    process_state_destroy(state);

    return res;
}

void rr_scheduler(char *shm_current_scheduled_block, char *shm_done[]) {
    struct timespec current_time;
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
        *shm_current_scheduled_block = (current_scheduled + new_schedule_offset) % 3;

        if (timespec_get(&current_time, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        double fmted_time = current_time.tv_sec + (current_time.tv_nsec / 1e9);
        printf("\n[%lf] scheduled: %d\n", fmted_time, *shm_current_scheduled_block);

        usleep(get_time_quantum());  // sleep for time quantum
    }
}

void fcfs_scheduler(char *shm_current_scheduled_block, char *shm_done[]) {
    // Iterate over each process, schedule them one by one.
    // Assumption: Ci arrived before Cj if i < j
    struct timespec current_time;
    for (int i = 0; i < 3; i++) {
        *shm_current_scheduled_block = i;

        if (timespec_get(&current_time, TIME_UTC) != TIME_UTC) {
            fprintf(stderr, "ERROR: call to timespec_get failed \n");
            exit(EXIT_FAILURE);
        }

        double fmted_time = current_time.tv_sec + (current_time.tv_nsec / 1e9);
        printf("\n[%lf] scheduled: %d\n", fmted_time, *shm_current_scheduled_block);

        while (*shm_done[i] == 0) {
            usleep(2000);  // check every 2ms if the process is completed or not
        }
    }
}
