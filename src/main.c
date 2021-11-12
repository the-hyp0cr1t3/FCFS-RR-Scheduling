#include <argp.h>
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
#include "process_state.h"
#include "scheduling.h"
#include "shared_memory.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    // Parse Arrguments

    int n1 = (int)(1e6), n2 = (int)(1e6), n3 = (int)(1e6);
    char scheduling_algorithm[8] = "fcfs";
    int time_quantum = __INT_MAX__;

    if (argc > 3) {
        n1 = atoi(argv[1]);
        n2 = atoi(argv[2]);
        n3 = atoi(argv[3]);
    }
    if (argc > 4) strcpy(scheduling_algorithm, argv[4]);
    if (strcmp(scheduling_algorithm, "rr") == 0) {
        time_quantum = (argc > 5) ? atoi(argv[5]) : DEFAULT_TIME_QUANTUM;
    }

    srand(time(NULL));

    // Check that all the required files are present. If not create them.
    check_file(SHM_CURRENT_SCHEDULED_FNAME);
    for (int i = 0; i < 3; ++i) {
        check_file(SHM_DONE[i]);
    }

    // Initialize the scheduling procedure. Perform pre schedule cleanup.
    sem_unlink(SEM_CPU_FNAME);
    sem_t* cpu_lock = sem_open(SEM_CPU_FNAME, O_CREAT, 0644, 1);
    if (cpu_lock == SEM_FAILED) {
        perror("ERROR: sem_open/cpu_lock failed\n");
        exit(EXIT_FAILURE);
    }

    pid_t child[3];

    int fd1[2];
    int p1 = pipe(fd1);
    if (p1 == -1) {
        fprintf(stderr, "Pipe-1 failed");
        return 1;
    }

    child[0] = fork();

    if (!child[0]) {  // Child Process 1 -- C1
        long long int sum[1] = {0};
        sum[0] = child_method(scheduling_algorithm, time_quantum, 0, cpu_lock, n1);
        close(fd1[READ]);  //writing sum to pipe
        write(fd1[WRITE], sum, sizeof(long long int));
        close(fd1[WRITE]);
    } else {
        int fd2[2];
        int p2 = pipe(fd2);

        if (p2 == -1) {
            fprintf(stderr, "Pipe-2 failed");
            return 1;
        }

        child[1] = fork();

        if (!child[1]) {  // Child Process 2 -- C2
            child_method(scheduling_algorithm, time_quantum, 1, cpu_lock, n2);
            char msg[] = "Done Printing";
            close(fd2[READ]);  //writing message to pipe
            write(fd2[WRITE], msg, strlen(msg) + 1);
            close(fd2[WRITE]);

        } else {
            int fd3[2];
            int p3 = pipe(fd3);

            if (p3 == -1) {
                fprintf(stderr, "Pipe-3 failed");
                return 1;
            }
            child[2] = fork();

            if (!child[2]) {  // Child Process 3 -- C3
                long long int s[1] = {0};
                s[0] = child_method(scheduling_algorithm, time_quantum, 2, cpu_lock, n3);
                close(fd3[READ]);  //writing sum to pipe
                write(fd3[WRITE], s, sizeof(long long int));
                close(fd3[WRITE]);
            } else {
                //  Parent Process -- M

                FILE* log_file = fopen(LOG_FNAME, "a");

                // Basic Logs
                fprintf(log_file, "Scheduling Algorithm: %s\n", scheduling_algorithm);
                fprintf(log_file, "Time Quntum: %d\n", time_quantum);
                fprintf(log_file, "Batch Size: %d\n", BATCH_SIZE);
                fprintf(log_file, "Process PIDS\n");
                fprintf(log_file, "M:\t%d\n", getpid());
                fprintf(log_file, "C1:\t%d\tn:\t%d\n", child[0], n1);
                fprintf(log_file, "C2:\t%d\tn:\t%d\n", child[1], n2);
                fprintf(log_file, "C3:\t%d\tn:\t%d\n", child[2], n3);
                fclose(log_file);

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
                    shm_done[i] = attach_memory_block(SHM_DONE[i], SHM_BLOCK_SIZE);
                    if (shm_done[i] == NULL) {
                        fprintf(stderr, "ERROR: Could not get block: %s\n", SHM_DONE[i]);
                        exit(EXIT_FAILURE);
                    }
                    *shm_done[i] = false;
                }

                // Schedule!
                if (strcmp(scheduling_algorithm, "rr") == 0) {
                    rr_scheduler(shm_current_scheduled_block, shm_done, time_quantum);
                } else if (strcmp(scheduling_algorithm, "fcfs") == 0) {
                    fcfs_scheduler(shm_current_scheduled_block, shm_done);
                } else {
                    fprintf(stderr, "ERROR: Invalid Scheduler. Try again with either fcfs or rr.\n");
                }

                wait(NULL);
                wait(NULL);
                wait(NULL);

                char mc2[15];
                long long int mc1[1];
                long long int mc3[1];
                close(fd1[WRITE]);  //read from pipe1
                read(fd1[READ], mc1, sizeof(long long int));
                close(fd1[READ]);

                close(fd2[WRITE]);  //read from pipe2
                read(fd2[READ], mc2, 15);
                close(fd2[READ]);

                close(fd3[WRITE]);  //read from pipe3
                read(fd3[READ], mc3, sizeof(long long int));
                close(fd3[READ]);

                printf("Child 1 Result: %lld\n", mc1[0]);
                printf("Child 2 Result: %s\n", mc2);
                printf("Child 3 Result: %lld\n", mc3[0]);

                detach_memory_block(shm_current_scheduled_block);

                if (destroy_memory_block(SHM_CURRENT_SCHEDULED_FNAME)) {
                    printf("Destroyed Block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                } else {
                    fprintf(stderr, "ERROR: Could not destroy block: %s\n", SHM_CURRENT_SCHEDULED_FNAME);
                }

                for (int i = 0; i < 3; i++) {
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
