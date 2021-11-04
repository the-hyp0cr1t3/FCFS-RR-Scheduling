#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include<string.h>
#include <sys/types.h>
#include<pthread.h>

#define C2_TXT "c2.txt"
#define C3_TXT "c3.txt"
#define READ 0
#define WRITE 1

#define N 1000000     // 1 million

// Return the sum of n random numbers
long long run_C1(int n);

// Read n integers from c2.txt and return "Done printing"
const char *run_C2(int n);

// Read n integers from c3.txt and return the sum
long long run_C3(int n);
