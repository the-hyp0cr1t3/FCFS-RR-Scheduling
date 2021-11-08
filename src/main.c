#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "process_state.h"
#include "scheduling.h"
#include "shared_memory.h"

int main(int argc, char *argv[]) {
    // Create a shared memory block

    // // Setup some semaphores Deal with it after doing without semaphores
    // sem_unlink(SEM_WRITER_FNAME);
    // sem_unlink(SEM_READER_FNAME);

    // sem_t *sem_writer = sem_open(SEM_WRITER_FNAME, IPC_CREAT, 0660, 0);
    // if (sem_writer == SEM_FAILED) {
    //     fprintf(stderr, "ERROR: sem_open/writer failed");
    //     exit(EXIT_FAILURE);
    // }

    // sem_t *sem_reader = sem_open(SEM_READER_FNAME, IPC_CREAT, 0660, 1);
    // if (sem_reader == SEM_FAILED) {
    //     fprintf(stderr, "ERROR: sem_open/reader failed");
    //     exit(EXIT_FAILURE);
    // }

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

                // Grab the shared memory block
                char *current_running_block = attach_memory_block(SHM_CURRENT_RUNNING_FILENAME, BLOCK_SIZE);
                if (current_running_block == NULL) {
                    fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_CURRENT_RUNNING_FILENAME);
                    exit(EXIT_FAILURE);
                }

                memcpy(current_running_block, (char *)(-1), sizeof(int)); // This might be a bug; Pls fix

                // Schedule!

                wait(NULL);
                wait(NULL);
                wait(NULL);

                detach_memory_block(current_running_block);

                if (destroy_memory_block(SHM_CURRENT_RUNNING_FILENAME)) {
                    printf("Destroyed Block: %s\n", SHM_CURRENT_RUNNING_FILENAME);
                } else {
                    fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_CURRENT_RUNNING_FILENAME);
                }
            }
        }
    }
}