#ifndef PROCESS_STATE
#define PROCESS_STAE

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct process_state {
    int id;
    bool done;
    int current_running_proc;  // Current Running proccess, determined by the scheduling algorithm and copied from the shared memory block
    char* shm_block;           // Shared Memory Block returned by shmget
    char sem_turn_fname[16];
    sem_t* cpu_lock;
    sem_t* turn_lock;
} process_state;

void unlink_semaphores();

process_state* process_state_init(int process_id, char* shm_filename, int shm_size, sem_t* cpu_lock);
void process_state_destroy(process_state* state);

#endif