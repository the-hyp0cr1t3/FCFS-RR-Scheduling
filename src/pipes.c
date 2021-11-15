#include "pipes.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"

void get_pipe(int fd[2]) {
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe Creation failed");
        exit(EXIT_FAILURE);
    }
}

void single_write(int fd[2], void* buff, int size) {
    close(fd[PIPE_READ]);  //writing sum to pipe
    write(fd[PIPE_WRITE], buff, size);
    close(fd[PIPE_WRITE]);
}

void single_read(int fd[2], void* buff, int size) {
    close(fd[PIPE_WRITE]);  //read from pipe1
    read(fd[PIPE_READ], buff, size);
    close(fd[PIPE_READ]);
}