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

                // Grab the shared memory block
                char *current_running_block = attach_memory_block(SHM_CURRENT_RUNNING_FNAME, BLOCK_SIZE);
                if (current_running_block == NULL) {
                    fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_CURRENT_RUNNING_FNAME);
                    exit(EXIT_FAILURE);
                }

                *current_running_block = -1;

                while (1) {
                    *current_running_block = (*current_running_block + 1) % 3;
                    printf("LOG [M]: current_running_block = %d\n", *current_running_block);
                    sleep(5);
                }

                // Schedule!

                wait(NULL);
                wait(NULL);
                wait(NULL);

                detach_memory_block(current_running_block);

                if (destroy_memory_block(SHM_CURRENT_RUNNING_FNAME)) {
                    printf("Destroyed Block: %s\n", SHM_CURRENT_RUNNING_FNAME);
                } else {
                    fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_CURRENT_RUNNING_FNAME);
                }
            }
        }
    }
}