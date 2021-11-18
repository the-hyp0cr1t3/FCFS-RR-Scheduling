#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "constants.h"
#include "parse_args.h"
#include "utils.h"
#include "pipes.h"
#include "shared_memory.h"
#include "scheduling.h"


int main(int argc, char* argv[]) {
    srand(time(NULL));

    // parse args and return n1, n2, n3, tq and scheduling algorithm
    arg_data args = parse_args(argc, argv);

    // create .shm files if they do not exist
    create_if_not_exist(SHM_CURRENT_SCHEDULED_FNAME);
    for (int i = 0; i < 3; i++)
        create_if_not_exist(SHM_DONE_FNAMES[i]);

    // create the CPU semaphore shared across processes
    //      ensures that the CPU can be used by only one process at a time
    sem_unlink(SEM_CPU_FNAME);
    sem_t *cpu_lock = sem_open(SEM_CPU_FNAME, O_CREAT, 0644, 1);
    if (cpu_lock == SEM_FAILED) {
        perror("ERROR: sem_open/cpu_lock failed\n");
        exit(EXIT_FAILURE);
    }

    // create pipes for inter process communication of results
    int fd1[2], fd2[2], fd3[2];
    get_pipe(fd1);
    get_pipe(fd2);
    get_pipe(fd3);

    // child processes C1, C2, C3
    pid_t child[3];
    child[0] = fork();

    if (!child[0]) {  // Child process 1 -- C1
        long long sum = child_method(0, cpu_lock, args.n1);
        single_write(fd1, &sum, sizeof sum);

    } else {
        child[1] = fork();

        if (!child[1]) {  // Child process 2 -- C2
            child_method(1, cpu_lock, args.n2);
            single_write(fd2, C2_DONE_MSG, strlen(C2_DONE_MSG) + 1);

        } else {
            child[2] = fork();

            if (!child[2]) {  // Child process 3 -- C3
                long long sum = child_method(2, cpu_lock, args.n3);
                single_write(fd3, &sum, sizeof sum);

            } else {
                //  Parent process -- M

                log_pids(getpid(), child, args);

                // grab the current scheduled shared memory block
                // this needs to be created and initialized before any scheduling starts
                char* shm_current_scheduled_block = attach_memory_block(SHM_CURRENT_SCHEDULED_FNAME, SHM_BLOCK_SIZE);
                if (shm_current_scheduled_block == NULL) {
                    fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                    exit(EXIT_FAILURE);
                }

                // -1 indicates that no process has been scheduled
                *shm_current_scheduled_block = -1;

                // create and initialize the done shared memory blocks to communicate with the Master process
                // no worker is currently running => hence the inconsistent values in these blocks before creation are acceptable
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
                if (args.scheduling_algorithm == RR) {
                    rr_scheduler(shm_current_scheduled_block, shm_done);
                } else {
                    fcfs_scheduler(shm_current_scheduled_block, shm_done);
                }


                long long c1_result, c3_result;
                char c2_result[strlen(C2_DONE_MSG) + 1];

                // get results from the children processes via the pipes
                single_read(fd1, &c1_result, sizeof c1_result);
                single_read(fd2, &c2_result, strlen(C2_DONE_MSG) + 1);
                single_read(fd3, &c3_result, sizeof c3_result);

                printf("Child 1 result: %lld\n", c1_result);
                printf("Child 2 result: %s\n", c2_result);
                printf("Child 3 result: %lld\n", c3_result);

                // wait for all children processes to finish execution and then proceed to post-processing and cleanup
                wait(NULL);
                wait(NULL);
                wait(NULL);

                // cleanup all shared memory blocks
                detach_memory_block(shm_current_scheduled_block);
                if (destroy_memory_block(SHM_CURRENT_SCHEDULED_FNAME)) {
                    printf("Destroyed block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                } else {
                    fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                }

                for (int i = 0; i < 3; i++) {
                    detach_memory_block(shm_done[i]);

                    if (destroy_memory_block(SHM_DONE_FNAMES[i])) {
                        printf("Destroyed block: %s\n", SHM_DONE_FNAMES[i]);
                    } else {
                        fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_DONE_FNAMES[i]);
                    }
                }
            }
        }
    }
}