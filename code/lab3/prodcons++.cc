#include <stdio.h>
#include "copyright.h"
#include "system.h"

#include "synch.h"
#include "ring.h"

#define BUFF_SIZE 3  // the size of the round buffer
#define N_PROD    2  // the number of producers 
#define N_CONS    2  // the number of consumers
#define N_MESSG   4  // the number of messages produced by each producer
#define MAX_NAME  16 // the maximum lengh of a name

#define MAXLEN    48
#define LINELEN    24

Thread *producers[N_PROD]; //array of pointers to the producer
Thread *consumers[N_CONS];  // and consumer threads;

char prod_names[N_PROD][MAX_NAME];  //array of charater string for prod names
char cons_names[N_CONS][MAX_NAME];  //array of charater string for cons names

Semaphore *nempty, *nfull; //two semaphores for empty and full slots
Semaphore *mutex;          //semaphore for the mutual exclusion
Ring *ring;


void Producer(_int which) {
//    printf("THE producer %d\n", which);
    for (int num = 0; num < N_MESSG; num++) {
        slot message(which, num);
        nempty->P();
        mutex->P();
        printf("in producer %d: %d\n", which, num);
        ring->Put(&message);
        mutex->V();
        nfull->V();
    }
}

void Consumer(_int which) {
    char str[MAXLEN];
    char fname[LINELEN];

    slot *message = new slot(0, 0); // 内存泄漏？

    sprintf(fname, "tmp_%d", which);

    FILE *fp = fopen(fname, "wa"); // 追加模式
    for (;;) {
        nfull->P();
        mutex->P();

        ring->Get(message);
        printf("in consumer %d, and get producer %d 's %d\n", which, message->thread_id, message->value);
        mutex->V();
        nempty->V();

        sprintf(str, "producer id --> %d; Message number --> %d;\n", message->thread_id, message->value);
        fprintf(fp, str);

    }

}

void ProdCons() {
    DEBUG('t', "Entering ProdCons");
    int i;
    nempty = new Semaphore("nempty", BUFF_SIZE);
    nfull = new Semaphore("nfull", 0);
    mutex = new Semaphore("mutex", 1);

    ring = new Ring(BUFF_SIZE);

    for (i = 0; i < N_PROD; i++) {
        sprintf(prod_names[i], "producer_%d", i);
        producers[i] = new Thread(prod_names[i]);
        printf("CREATE producer %d\n", i);
        producers[i]->Fork(Producer, i);

    };

    for (i = 0; i < N_CONS; i++) {
        sprintf(cons_names[i], "consumer_%d", i);
        consumers[i] = new Thread(cons_names[i]);
        consumers[i]->Fork(Consumer, i);
    };
}

