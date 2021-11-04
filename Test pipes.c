#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#define READ 0
#define WRITE 1
#define BUFF_SIZE 100

int main() {
    srand(time(NULL));
    int fd[2], fd2[2];
    char buff[BUFF_SIZE];

    if(pipe(fd)) {
        perror("Pipe fd could not be created");
        exit(1);
    }

    if(pipe(fd2)) {
        perror("Pipe fd2 could not be created");
        exit(1);
    }

    if(!fork()) {   // child
        // active: fd[READ], fd2[WRITE]
        close(fd[WRITE]);
        close(fd2[READ]);

        while(1) {
            read(fd[READ], buff, BUFF_SIZE);

            int x = atoi(buff);
            printf("child reads %d\n", x);
            fflush(stdout);

            if(x == 7) {
                write(fd2[WRITE], "Lucky", 6);
                break;
            }

            write(fd2[WRITE], "Unlucky", 8);
        }

        close(fd[READ]);
        close(fd2[WRITE]);
    } else {    // parent
        // active: fd[WRITE], fd2[READ]
        close(fd[READ]);
        close(fd2[WRITE]);

        int iters = 0;
        while(1) {
            int x = rand() % 10 + 1;
            sprintf(buff, "%d", x);
            write(fd[WRITE], buff, BUFF_SIZE);

            read(fd2[READ], buff, BUFF_SIZE);
            printf("parent reads \"%s\"\n\n", buff);
            fflush(stdout);

            if(!strcmp(buff, "Lucky"))
                break;
            iters++;
        }

        printf("%d iterations\n", iters);
        fflush(stdout);

        close(fd[WRITE]);
        close(fd2[READ]);

        wait(NULL);
    }
}