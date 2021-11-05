#ifndef QUEUE
#define QUEUE

#include <stdbool.h>
#include <stdlib.h>

#define QUEUE_SIZE 3

typedef struct {
    int data[QUEUE_SIZE];
    int front, rear;
} Queue;

Queue* create_queue();
bool enque(Queue*, int);
int dequeue(Queue*);
void delete_queue(Queue*);

#endif