#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "pipes.h"

void get_pipe(int fd[2]) {
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe Creation failed");
        exit(EXIT_FAILURE);
    }
}

void single_write(int fd[2], void *buff, size_t size) {
    close(fd[PIPE_READ]);
    write(fd[PIPE_WRITE], buff, size);
    close(fd[PIPE_WRITE]);
}

void single_read(int fd[2], void *buff, size_t size) {
    close(fd[PIPE_WRITE]);
    read(fd[PIPE_READ], buff, size);
    close(fd[PIPE_READ]);
}