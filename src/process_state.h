#ifndef PROCESS_STATE
#define PROCESS_STATE

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct process_state {
    int id;                      /** < Process ID */
    bool done;                   /** < Done status of the task. */
    int current_scheduled;       /** < Current Scheduled proccess, determined by the scheduling algorithm and copied from the shared memory block */
    char* shm_current_scheduled; /** < Shared Memory Block returned by shmget */
    char* shm_done;              /** < Shared Memory Block returned by shmget */
    char sem_turn_fname[16];     /** < Name to use for the named semaphore, turn */
    sem_t* cpu_lock;             /** < Semaphore to protect the CPU (as a sparse resource) */
    sem_t* turn_lock;            /** < Semaphore to determine if it is this process's turn */
    int n;                       /** < workload of the process */
    long long int result;        /** < result of the process */
} process_state;

process_state* process_state_init(int process_id, sem_t* cpu_lock, int num);
void process_state_destroy(process_state* state);

#endif