#include <stdio.h>
#include "a2.h"
#include "queue.h"

void buildQueue(priorityQ * pq, int length) {
    pq->count = 0;
    pq->size = length;
}

//Borrowed algorithm for maxHeapify from user aatishnn on GitHub: https://gist.github.com/aatishnn/8265656

void maxHeapify(int * data, process a[], int cur, int count) {

    int left, right, largest, temp;
    left = 2 * (cur) + 1;
    right = left + 1;
    largest = cur;

    if (left <= count && a[data[left]].curPrior > a[data[largest]].curPrior)
        largest = left;

    if (right <= count && a[data[right]].curPrior > a[data[largest]].curPrior)
        largest = right;

    if (largest != cur) {
        temp = data[cur];
        data[cur] = data[largest];
        data[largest] = temp;
        maxHeapify(data, a, largest, count);
    }
}

void enqueue(priorityQ * pq, process a[], int value) {

    int index, parent;
    index = pq->count++;
    parent = (index - 1) / 2;

    while ((index != 0) && (a[value].curPrior > a[pq->pqueue[parent]].curPrior)) {
        swap(pq, index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
    pq->pqueue[index] = value;
}

void swap(priorityQ * pq, int index, int parent) {
    int temp = pq->pqueue[index];
    pq->pqueue[index] = pq->pqueue[parent];
    pq->pqueue[parent] = temp;
}

int dequeue(priorityQ * pq, process a[]) {

    int dequeue;
	int temp = pq->pqueue[pq->count - 1];
    pq->count--;

 	dequeue = pq->pqueue[0];
 	pq->pqueue[0] = temp;
 	maxHeapify(pq->pqueue, a, 0, pq->count);

 	return dequeue;
}
