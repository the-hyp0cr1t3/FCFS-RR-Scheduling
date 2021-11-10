#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process_state.h"
#include "scheduling.h"
#include "shared_memory.h"

#define SEM_CPU_FNAME "/cpu"

int main(int argc, char *argv[]) {
    // Create a shared memory block
    int time_quantum = 5;

    sem_unlink(SEM_CPU_FNAME);
    sem_t *cpu_lock = sem_open(SEM_CPU_FNAME, O_CREAT, 0644, 1);
    if (cpu_lock == SEM_FAILED) {
        perror("ERROR: sem_open/cpu_lock failed\n");
        exit(EXIT_FAILURE);
    }

    pid_t child[3];

    child[0] = fork();

    if (!child[0]) {  // Child Process 1 -- C1
        child_method(0, cpu_lock);
    } else {
        child[1] = fork();

        if (!child[1]) {  // Child Process 2 -- C2
            child_method(1, cpu_lock);
        } else {
            child[2] = fork();

            if (!child[2]) {  // Child Process 3 -- C3
                child_method(2, cpu_lock);
            } else {
                //  Parent Process -- M

                printf("Process PIDS\nM:\t%d\nC1:\t%d\nC2:\t%d\nC3:\t%d\n", getpid(), child[0], child[1], child[2]);

                // Grab the shared memory block
                char *shm_current_scheduled_block = attach_memory_block(SHM_CURRENT_SCHEDULED_FNAME, BLOCK_SIZE);
                if (shm_current_scheduled_block == NULL) {
                    fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                    exit(EXIT_FAILURE);
                }

                *shm_current_scheduled_block = -1;


                char *shm_done[3];

                for(int i = 0; i < 3; i++) {
                    shm_done[i] = attach_memory_block(SHM_DONE[i], BLOCK_SIZE);
                    if (shm_done[i] == NULL) {
                        fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_DONE[i]);
                        exit(EXIT_FAILURE);
                    }
                    *shm_done[i] = false;
                }


                // Good Times :')



                if (strcmp(argv[1], "rr") == 0) {
                    rr_scheduler(shm_current_scheduled_block, time_quantum, shm_done);
                } else {
                    fcfs_scheduler(shm_current_scheduled_block,shm_done);
                }

                // Schedule!

                wait(NULL);
                wait(NULL);
                wait(NULL);

                detach_memory_block(shm_current_scheduled_block);

                if (destroy_memory_block(SHM_CURRENT_SCHEDULED_FNAME)) {
                    printf("Destroyed Block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                } else {
                    fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                }

                for(int i = 0; i < 3; i++) {
                    detach_memory_block(shm_done[i]);

                    if (destroy_memory_block(SHM_DONE[i])) {
                        printf("Destroyed Block: %s\n", SHM_DONE[i]);
                    } else {
                        fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_DONE[i]);
                    }
                }
            }
        }
    }
}