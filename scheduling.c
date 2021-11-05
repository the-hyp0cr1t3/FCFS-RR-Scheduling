#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "queue.h"

typedef struct {
    Queue *running;
    Queue *waiting;
    Queue *finished;
} process_state;

void monitor(void *args) {
    process_state *state = (process_state *)args;

    // Update the state variable from the shared memory
}

void worker(void *args) {
    process_state *state = (process_state *)args;
}

void child_method(int process_id) {
    /*

    The monitor thread reads value from the shared memory and regulary updates the state variable. 
    The state variable holds data about waiting processes, the currently executing process and finished processes.
    Only the main method can write to the shared memory. And only the monitor threads of all children can read the shared memory space. 
    This prevents the need for locking, by preventing any race conditions.

    */

    process_state *state;
    pthread_t m_id, w_id;  // Monitor and Worker Thread IDs

    pthread_create(&m_id, NULL, monitor, (void *)state);
    pthread_create(&w_id, NULL, worker, (void *)state);

    while (1) {
        printf("Child %d\n", process_id);
        // sleep(1);
    }

    pthread_join(m_id, NULL);
    pthread_join(w_id, NULL);
}

int main(int argc, char *argv[]) {
    // Create a shared memory block


    key_t key = ftok("sate.shm", 0);


    pid_t child[3];

    child[0] = fork();

    if (!child[0]) {  // Child Process 1 -- C1
        child_method(0);
    } else {
        child[1] = fork();

        if (!child[1]) {  // Child Process 2 -- C2
            child_method(1);
        } else {
            child[2] = fork();

            if (!child[2]) {  // Child Process 3 -- C3
                child_method(2);
            } else {
                //  Parent Process -- M

                // Schedule!

                wait(NULL);
                wait(NULL);
                wait(NULL);
            }
        }
    }
}