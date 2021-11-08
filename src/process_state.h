#ifndef PROCESS_STATE
#define PROCESS_STAE

#include <pthread.h>
#include <stdbool.h>

typedef struct process_state {
    int id;
    bool done;
    int current_running_proc;  // Current Running proccess, determined by the scheduling algorithm and copied from the shared memory block
    char* shm_block;           // Shared Memory Block returned by shmget
    pthread_cond_t cond;
    pthread_mutex_t lock;
} process_state;

void process_state_init(process_state* state, int process_id, char* shm_filename, int shm_size);
void process_state_destroy(process_state* state);

#endif