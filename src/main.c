#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "constants.h"
#include "context_manager.h"
#include "pipes.h"
#include "process_state.h"
#include "scheduling.h"
#include "shared_memory.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    // Parse Arrguments
    task ts = argparse(argc, argv);
    srand(time(NULL));

    // Check that all the required files are present. If not create them.
    check_file(SHM_CURRENT_SCHEDULED_FNAME);
    for (int i = 0; i < 3; ++i) {
        check_file(SHM_DONE_FNAMES[i]);
    }

    // Create the CPU Semaphore shared across processes. 
    // Ensures that the CPU can be used by only task thread at a time.    
    sem_unlink(SEM_CPU_FNAME);
    sem_t* cpu_lock = sem_open(SEM_CPU_FNAME, O_CREAT, 0644, 1);
    if (cpu_lock == SEM_FAILED) {
        perror("ERROR: sem_open/cpu_lock failed\n");
        exit(EXIT_FAILURE);
    }

    // Create the pipes for inter process communication of results
    int fd1[2], fd2[2], fd3[2];
    get_pipe(fd1);
    get_pipe(fd2);
    get_pipe(fd3);

    pid_t child[3];
    child[0] = fork();

    if (!child[0]) {  // Child Process 1 -- C1
        long long int* sum = malloc(sizeof(int));
        *sum = child_method(0, cpu_lock, ts.n1);
        single_write(fd1, sum, sizeof(long long int));
    } else {
        child[1] = fork();

        if (!child[1]) {  // Child Process 2 -- C2
            char msg[] = "Done Printing";
            child_method(1, cpu_lock, ts.n2);
            single_write(fd2, msg, strlen(msg) + 1);

        } else {
            child[2] = fork();

            if (!child[2]) {  // Child Process 3 -- C3
                long long int* sum = malloc(sizeof(int));
                *sum = child_method(2, cpu_lock, ts.n3);
                single_write(fd3, sum, sizeof(long long int));
            } else {
                //  Parent Process -- M

                log_pids(getpid(), child, ts);

                // Grab the current scheduled shared memory block.
                // This needs to be created and initialized before any scheduling starts.
                char* shm_current_scheduled_block = attach_memory_block(SHM_CURRENT_SCHEDULED_FNAME, SHM_BLOCK_SIZE);
                if (shm_current_scheduled_block == NULL) {
                    fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                    exit(EXIT_FAILURE);
                }

                // -1 indicates that no process has been scheduled.
                *shm_current_scheduled_block = -1;

                // Create and Initialize the done shared memory blocks to communicate with the Master Process.
                // No worker is currently running, and hence the inconsistent values in these blocks before creation are acceptable.
                char* shm_done[3];
                for (int i = 0; i < 3; i++) {
                    shm_done[i] = attach_memory_block(SHM_DONE_FNAMES[i], SHM_BLOCK_SIZE);
                    if (shm_done[i] == NULL) {
                        fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_DONE_FNAMES[i]);
                        exit(EXIT_FAILURE);
                    }
                    *shm_done[i] = false;
                }

                // Schedule!
                if (strcmp(get_scheduling_algorithm(), "rr") == 0) {
                    rr_scheduler(shm_current_scheduled_block, shm_done);
                } else if (strcmp(get_scheduling_algorithm(), "fcfs") == 0) {
                    fcfs_scheduler(shm_current_scheduled_block, shm_done);
                } else {
                    fprintf(stderr, "ERROR: Invalid Scheduler. Try again with either fcfs or rr.\n");
                    exit(EXIT_FAILURE);
                }

                // Wait for all the children processes to finish execution and then proceed to post-processing and cleanup
                wait(NULL);
                wait(NULL);
                wait(NULL);

                long long int mc1[1], mc3[1];
                char mc2[16];

                // Get the results of the task from the children processes via the pipes
                single_read(fd1, mc1, sizeof(long long int));
                single_read(fd2, mc2, 16);
                single_read(fd3, mc3, sizeof(long long int));

                printf("Child 1 Result: %lld\n", mc1[0]);
                printf("Child 2 Result: %s\n", mc2);
                printf("Child 3 Result: %lld\n", mc3[0]);

                // Cleanup all the Shared Memory blocks
                detach_memory_block(shm_current_scheduled_block);
                if (destroy_memory_block(SHM_CURRENT_SCHEDULED_FNAME)) {
                    printf("Destroyed Block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                } else {
                    fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                }

                for (int i = 0; i < 3; i++) {
                    detach_memory_block(shm_done[i]);

                    if (destroy_memory_block(SHM_DONE_FNAMES[i])) {
                        printf("Destroyed Block: %s\n", SHM_DONE_FNAMES[i]);
                    } else {
                        fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_DONE_FNAMES[i]);
                    }
                }
            }
        }
    }
}