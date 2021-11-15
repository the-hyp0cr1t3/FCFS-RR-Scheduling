#ifndef PIPES
#define PIPES

void get_pipe(int fd[2]);
void single_write(int fd[2], void* buff, int size);
void single_read(int fd[2], void* buff, int size);

#endif