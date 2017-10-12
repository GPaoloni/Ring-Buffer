#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "datatype.h"

// READ: the buffer size MUST be a power of 2 (4, 8 16,.., 1024..)
typedef struct _RingBuffer
{
    int size;
    DataType *values;      //buffer
    int inPointer;
    int outPointer;
    int isEmpty;
    pthread_mutex_t mutex;
#ifdef SAFEWRITE
    sem_t readsem, writesem; //used to avoid overwriting if buffer is full
#else
    pthread_cond_t cond;     //used to avoid read from empty buffer
#endif
}   RingBuffer;

//Initialize ring buffer of size sz
RingBuffer* rb_init(int);

//Returns 1 if buffer is empty, 0 otherwise. Only for intern use of the structure
int rb_isEmpty(RingBuffer*);

//Returns 1 if buffer is full, 0 otherwise. Only for intern use if the structure
int rb_isFull(RingBuffer*);

//Enque an element to the RingBuffer. Returns 1 if succes, 0 otherwise
int rb_enque(RingBuffer*, DataType);

//Deque the first element of the RingBuffer. Returns that element
DataType rb_deque(RingBuffer*);

//Returns the number of remaining items in the buffer
int rb_remaining(RingBuffer*);

//Destroys the RingBuffer
void rb_destroy(RingBuffer*);

#endif
