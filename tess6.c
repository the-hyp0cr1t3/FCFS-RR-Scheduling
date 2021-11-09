#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

bool done = false;
bool allowed = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv;


void *worker_thread_func(void *args) {
    int i = 1, n = 3;
    while(!done) {
        while(!allowed);

        while(i <= n) {
            if(!allowed) {
                // cleanup
                break;
            }
            printf("Worker prints %d\n", i);
            sleep(1);
            i++;
        }

        if(i > n) {
            done = true;
            printf("Worker done\n");
        }

        pthread_cond_signal(&cv);
    }

}

void *monitor_thread_func(void *args) {

    pthread_t worker_id;
    pthread_create(&worker_id, NULL, worker_thread_func, NULL);

    while(!done) {

        int retval;     // for error codes

        // Acquire mutex lock over condition variable
        if((retval = pthread_mutex_lock(&mutex))) {
            printf("pthread_mutex_lock %s\n", strerror(retval));
            exit(EXIT_FAILURE);
        }

        printf("starting...\n");

        // some time struct for pthread condtional timed-wait
        struct timespec to;
        pthread_condattr_t attr;

        pthread_condattr_init(&attr);
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_cond_init(&cv, &attr);

        clock_gettime(CLOCK_MONOTONIC, &to);
        to.tv_sec += 5;         // 5 seconds from now


        // allowed true -> worker should be working
        allowed = true;

        // block and wait till wither the worker completes task or 5 seconds pass
        retval = pthread_cond_timedwait(&cv, &mutex, &to);

        if(retval == 0) {
            printf("Worker completed ahead of time.\n");
        } else if(retval == ETIMEDOUT) {
            printf("Time is up, but worker thread could not finish.\n");
        } else {
            printf("pthread_cond_timedwait %s\n", strerror(retval));
            exit(EXIT_FAILURE);
        }

        // stop the worker
        allowed = false;

        // release lock
        if((retval = pthread_mutex_unlock(&mutex))) {
            printf("pthread_mutex_unlock %s\n", strerror(retval));
            exit(EXIT_FAILURE);
        }

    }

    pthread_join(worker_id, NULL);
    printf("Monitor done\n");
}

int main() {
    pthread_t monitor_id;

    // create monitor and worker threads
    pthread_create(&monitor_id, NULL, monitor_thread_func, NULL);

    pthread_join(monitor_id, NULL);
}