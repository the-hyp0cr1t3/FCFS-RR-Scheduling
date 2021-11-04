#include "OS_assn.h"

/* TODO: Fork and use pipes; Call respective functions */

int main() {
    srand(time(NULL));
    long long int n1,n2,n3;
    printf("Enter value of n1: ");
    scanf("%lld",&n1);
    printf("Enter value of n2: ");
    scanf("%lld",&n2);
    printf("Enter value of n3: ");
    scanf("%lld",&n3);

    pid_t c1,c2,c3;

    int fd1[2];
    int p1 = pipe(fd1);
    if(p1 == -1){
        fprintf(stderr,"Pipe-1 failed");
        return 1;
    }
    c1 = fork();

    if(c1 < 0){
        fprintf(stderr,"Fork-1 failed");
        return 1;
    }

    if(c1 == 0){            //  C1-adds n1 numbers
        long long int sum[1]={0};
        sum[0]=run_C1(n1);
        close(fd1[READ]);       //writing sum to pipe
        write(fd1[WRITE], sum, sizeof(long long int));
        close(fd1[WRITE]);
    }

    else{

        int fd2[2];
        int p2 = pipe(fd2);

        if(p2 == -1){
            fprintf(stderr,"Pipe-2 failed");
            return 1;
        }

        c2 = fork();

        if(c2 < 0){
            fprintf(stderr,"Fork-2 failed");
            return 1;
        }

        if(c2 == 0){        //  C2-reads and prints n2 numbers

            const char *msg = run_C2(n2);
            close(fd2[READ]);       //writing message to pipe
            write(fd2[WRITE], msg, strlen(msg)+1);
            close(fd2[WRITE]);

        }

        else{

            int fd3[2];
            int p3 = pipe(fd3);

            if(p3 == -1){
                fprintf(stderr,"Pipe-3 failed");
                return 1;
            }

            c3 = fork();

            if(c3 < 0){
                fprintf(stderr,"Fork-3 failed");
                return 1;
            }

            if(c3 == 0){    //  C3-reads n3 numbers and adds
                long long int s[1]={0};
                s[0] = run_C3(n3);;
                close(fd3[READ]);       //writing sum to pipe
                write(fd3[WRITE], s,sizeof(long long int));
                close(fd3[WRITE]);
            }

            else{           //  M (parent)

                wait(NULL);
                wait(NULL);
                wait(NULL);

                char mc2[15];
                long long int mc1[1];
                long long int mc3[1];
                close(fd1[WRITE]);              //read from pipe1
                read(fd1[READ], mc1,sizeof(long long int));
                close(fd1[READ]);

                close(fd2[WRITE]);           //read from pipe2
                read(fd2[READ], mc2,15);
                close(fd2[READ]);

                close(fd3[WRITE]);            //read from pipe3
                read(fd3[READ], mc3,sizeof(long long int));
                close(fd3[READ]);

                printf("Child 1: %lld\n",mc1[0]);
                printf("Child 2: %s\n",mc2);
                printf("Child 3: %lld\n",mc3[0]);

            }
        }
    }

    return 0;

}

// gcc partA.c child_functions.c -o a.out
