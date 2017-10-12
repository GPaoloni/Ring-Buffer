#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "ringbuffer.h"

#define BUFFER_SIZE 8       //MUST be a power of 2 (readme)
#define MAX_PROD 10        //amount of producers threads (escriben a buffer)
#define MAX_CONS 3         //amount of consumers threads (leen desde buffer)
   

RingBuffer* buff;
int totalwrites, totalreads, finish;
pthread_mutex_t writemutex, readmutex;  

void *writeToBuff(void *pid)
{
    DataType value;
    int i, wrote;
    for(i=0; i<15; i++) {
        value.x = *(int *)pid;
        value.y = i;
        wrote = rb_enque(buff, value);     

        if(wrote){
            pthread_mutex_lock(&writemutex);
            totalwrites++;
            pthread_mutex_unlock(&writemutex);
            printf("Thread %d escribió %d\n", value.x, value.y);      
        sleep(rand() % 3);
        }
    }
}

void *readFromBuff()
{
    DataType val;
//    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); commented cause issue was solved

    sleep(2);            //this is here to test how the buffer works when it is full
    while(!finish) {
        val = rb_deque(buff);
        pthread_mutex_lock(&readmutex);
        totalreads++;
        pthread_mutex_unlock(&readmutex);
        printf("                               Leído: (%d, %d)\n", val.x, val.y);
    }
}

int main()
{
    totalwrites = 0;
    totalreads = 0;
    finish = 0;
    pthread_mutex_init(&writemutex, NULL);
    pthread_mutex_init(&readmutex, NULL);
    buff = rb_init(BUFFER_SIZE);
    pthread_t consumers[MAX_CONS], producers[MAX_PROD];
    int i, pid[MAX_PROD];

    for(i=0; i<MAX_PROD; i++) {
        pid[i] = i;
        pthread_create(&producers[i], NULL, writeToBuff, (void*)&pid[i]);
    }

    for(i=0; i<MAX_CONS; i++) {
    pthread_create(&consumers[i], NULL, readFromBuff, NULL);
    }
    
    for(i=0; i<MAX_PROD; i++){
        pthread_join(producers[i], NULL);
    }

    finish = 1;                                 
/*    for(i=0; i<MAX_CONS; i++){
        pthread_cancel(consumers[i]);           //Terminates consumers
    }                                           //Note that this is implemented this way, because there may
*/                                              //be threads waiting in the pthread_cond_wait of rb_deque
                                                //commented cause issue was solved
    int rem = rb_remaining(buff);
    printf("Quedaron %d elementos en el buffer\n", rem);

    printf("Cantidad total de escrituras en el buffer %d\n", totalwrites);
    printf("Cantidad total de lecturas %d\n", totalreads);

    rb_destroy(buff);
    return 0;
}
