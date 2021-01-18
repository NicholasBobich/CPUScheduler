/*
Author: Nick Bobich
Class: Operating Systems
Assignment 2: CPU Scheduler
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "a2.h"
#include "queue.h"

void readFile(process a[], int index[], FILE * infile, int * length);
void cpuToIo(priorityQ * pq, process a[], int io[], int cpu, int * ioCount);
void checkCpu(priorityQ * pq, os OS, process a[], int io[], int * cpu, int * ioCount);
void toReady(priorityQ * pq, process a[], int index);
void checkReady(priorityQ * pq, os OS, process a[], int * cpu, int length);
void readyToCpu(priorityQ * pq, process a[], int * cpu);
void ioToReady(priorityQ * pq, int io[], process a[], int * ioCount, int length);
void processFinished(priorityQ * pq, process a[], int ioCount);

int main(int argc, char * argv[]) {

    int ret = 0;
    FILE * infile;
    char * fname = argv[1];
    int length = 0;

    process a[48];
    int index[48];
    priorityQ * pq = (priorityQ *)malloc(sizeof(priorityQ));

    os OS;
    OS.quantum = 70;
    OS.wait = 30;

    int io[48];
    int cpu = 48;
    int ioCount, activeCount = 0;

    if ((infile = fopen(fname, "r")) == NULL) {
		printf("file not found\n");
		ret--;
    }

    readFile(a, index, infile, &length);
    buildQueue(pq, length);
    toReady(pq, a, activeCount);                          //Insert first process so that active count is one
    activeCount++;

    while (pq->count > 0) {
        while (activeCount < length) {

            a[activeCount].curCpu = 0;
        	a[activeCount].curIo = 0;
        	a[activeCount].wait = 0;
        	a[activeCount].curPrior = 0;
        	a[activeCount].cpuTotal = 0;
        	a[activeCount].ioTotal = 0;
        	a[activeCount].waitSum = 0;
        	a[activeCount].waitCount = 0;
        	a[activeCount].waitMin = 0;
        	a[activeCount].waitMax = 0;

            toReady(pq, a, activeCount);
            activeCount++;
        }

        checkReady(pq, OS, a, &cpu, length);
        checkCpu(pq, OS, a, io, &cpu, &ioCount);
        ioToReady(pq, io, a, &ioCount, length);
        processFinished(pq, a, ioCount);                //Wrote this method to dequeue processes once their cpu time and io time are
                                                        //  complete. This was the only way I could think of to prevent them from
                                                        //  bouncing back and forth between the ready queue, cpu and io until runtime
                                                        //  was finished.

    }

    printStats(a, OS);

    free(pq);
    fclose(infile);
    return ret;
}

void readFile(process a[], int index[], FILE * infile, int * length) {

    char buf[4096];
    char *token;
    int i = 0;
    int ch;

    while ((ch = fgetc(infile)) != EOF) {
        if (i == 0)
            ungetc(ch, infile);
        fscanf(infile,"%[^\n]", buf);
        token = strtok(buf, " ");
        int token_num = 1;
        while (token != NULL) {
            switch(token_num) {
                case(1):
                    sscanf(token, "%u", &a[i].priority);
                    break;
                case(2):
                    sscanf(token, "%u", &a[i].cpu);
                    break;
                case(3):
                    sscanf(token, "%u", &a[i].io);
                    break;
                case(4):
                    sscanf(token, "%u", &a[i].runTime);
                    break;
            }
            token = strtok(NULL, " ");
            token_num++;
        }

        if (i != 48)                        //Storing indexes which will be used to reference process array
            index[i] = i;

        i++;
    }
    *length = i - 1;
}

void cpuToIo(priorityQ * pq, process a[], int io[], int cpu, int * ioCount) {
    a[cpu].cpuTotal = a[cpu].cpuTotal + a[cpu].curCpu;
    a[cpu].curCpu = 0;
    a[cpu].curIo = 0;
    io[*ioCount] = cpu;
    *ioCount += 1;
}

void checkCpu(priorityQ * pq, os OS, process a[], int io[], int * cpu, int * ioCount) {
    if (*cpu != 48) {
        a[*cpu].curCpu++;
        if (a[*cpu].curCpu == a[*cpu].cpu) {
            cpuToIo(pq, a, io, *cpu, ioCount);
            *cpu = 48;
        }
        if (a[*cpu].curCpu == OS.quantum && a[*cpu].curCpu < a[*cpu].cpu) {
            toReady(pq, a, *cpu);
            *cpu = 48;
        }
    }
}

void toReady(priorityQ * pq, process a[], int index) {
    a[index].waitCount += 1;
    a[index].wait = 0;
    a[index].curPrior = a[index].priority;
    enqueue(pq, a, index);
}

void checkReady(priorityQ * pq, os OS, process a[], int * cpu, int length) {
    if (*cpu == 48 && a[pq->pqueue[0]].cpuTotal != a[pq->pqueue[0]].cpu)
        readyToCpu(pq, a, cpu);

    for (int i = 0; i < pq->count; i++) {
        a[pq->pqueue[i]].wait++;
        if ((a[pq->pqueue[i]].wait % OS.wait) == 0 && a[pq->pqueue[i]].curPrior < 15)
            a[pq->pqueue[i]].curPrior++;
    }

    maxHeapify(pq->pqueue, a, 0, pq->count);
}

void readyToCpu(priorityQ * pq, process a[], int * cpu) {
    int pid;
    pid = dequeue(pq, a);
    *cpu = pid;

    if (a[pid].waitSum == 0) {
        a[pid].waitSum = a[pid].waitSum + a[pid].wait;
        a[pid].waitMin = a[pid].wait;
        a[pid].waitMax = a[pid].wait;

    } else {
        a[pid].waitSum = a[pid].waitSum + a[pid].wait;

        if (a[pid].wait < a[pid].waitMin)
            a[pid].waitMin = a[pid].wait;

        if (a[pid].wait > a[pid].waitMax)
            a[pid].waitMax = a[pid].wait;
    }
}

void ioToReady(priorityQ * pq, int io[], process a[], int * ioCount, int length) {
    int pid;
    for (int j = 0; j < *ioCount; j++) {
        a[io[j]].curIo++;
        if (a[io[j]].curIo == a[io[j]].io) {
            a[io[j]].ioTotal = a[io[j]].curIo;
            pid = io[j];
            for (int i = j; i < length - 1; i++) {
                io[i] = io[i + 1];
            }
            *ioCount -= 1;
            a[pid].curIo = 0;
            toReady(pq, a, pid);
        }
    }
}

void processFinished(priorityQ * pq, process a[], int ioCount) {
    if (a[pq->pqueue[0]].cpuTotal == a[pq->pqueue[0]].cpu && a[pq->pqueue[0]].ioTotal == a[pq->pqueue[0]].io && ioCount == 0)
        dequeue(pq, a);
}
