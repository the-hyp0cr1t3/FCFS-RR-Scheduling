#include "queue.h"

#include <stdlib.h>

Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = 0;
    q->rear = 0;

    return q;
}

bool enque(Queue* q, int val) {
    // Invalid values
    if (q->rear >= QUEUE_SIZE || q->rear < 0) return 0;

    q->data[q->rear++] = val;
    q->rear %= QUEUE_SIZE;

    return 1;
}

int dequeue(Queue* q) {
    int v = q->data[q->front++];
    q->front %= QUEUE_SIZE;
    return v;
}

void delete_queue(Queue* q) {
    free(q);
    q = NULL;
}