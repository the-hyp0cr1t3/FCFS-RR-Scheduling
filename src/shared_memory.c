#include "shared_memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define IPC_RESULT_ERROR (-1)

static int get_shared_block(const char *filename, const int size) {
    key_t key;

    // Request a ket
    // The key is linked to a filename, so that other programs can access it
    key = ftok(filename, 0);  // 0 gives the first block on the file ig?

    if (key == IPC_RESULT_ERROR) {
        return IPC_RESULT_ERROR;
    }

    // Get shared block -- Create if it does not exist
    return shmget(key, size, 0644 | IPC_CREAT);
}

char *attach_memory_block(const char *filename, const int size) {
    int shared_block_id = get_shared_block(filename, size);
    char *result;

    if (shared_block_id == IPC_RESULT_ERROR) {
        return NULL;
    }

    // map the shared block into this process's memory
    // and give a pointer to it
    result = shmat(shared_block_id, NULL, 0);
    if (result == (char *)IPC_RESULT_ERROR) {
        return NULL;
    }

    return result;
}

bool detach_memory_block(const char *block) {
    return (shmdt(block) != IPC_RESULT_ERROR);
}

bool destroy_memory_block(const char *filename) {
    int shared_block_id = get_shared_block(filename, 0);

    if (shared_block_id == IPC_RESULT_ERROR) {
        return false;
    }

    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}