#ifndef QUEUE_H
#define QUEUE_H

struct heap {
    int count;
    int size;
    int pqueue[48];
};
typedef struct heap priorityQ;

void buildQueue(priorityQ * pq, int length);
void maxHeapify(int * data, process a[], int cur, int count);
void enqueue(priorityQ * pq, process a[], int value);
int dequeue(priorityQ * pq, process a[]);
void swap(priorityQ * pq, int a, int b);

#endif
