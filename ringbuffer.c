#include <stdio.h>
#include <string.h>
#include "ringbuffer.h"

//sz is the size of buffer. It must be a power of 2!!
RingBuffer* rb_init(int sz)
{
    RingBuffer *buff = malloc(sizeof(RingBuffer));
    buff->size = sz;
    buff->values = malloc(sizeof(DataType)*sz);
    buff->inPointer = 0;
    buff->outPointer = 0;
    buff->isEmpty = 1;
    pthread_mutex_init(&buff->mutex, NULL);
#ifdef SAFEWRITE
    sem_init(&buff->readsem, 0, 0);
    sem_init(&buff->writesem, 0, sz);
#else
    buff->cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
#endif
    return buff;
}

int rb_isEmpty(RingBuffer* buff)
{
    return buff->isEmpty;
}

int rb_isFull(RingBuffer* buff)
{
    return (!(buff->isEmpty) && (buff->inPointer==buff->outPointer));
}

int rb_enque(RingBuffer *buff, DataType val)
{
#ifdef SAFEWRITE //waits for a free-spot in the buffer for store the arg value
                 //if not eneabled, information may be lost when trying to write in a full-bufer
    sem_wait(&buff->writesem);
    pthread_mutex_lock(&buff->mutex);
    buff->values[buff->inPointer++ & (buff->size-1)] = val; //TODO (ref 1 readme)
    pthread_mutex_unlock(&buff->mutex);
    return sem_post(&buff->readsem) ? 0 : 1;    //return 1 if ok 
#else 
    pthread_mutex_lock(&buff->mutex);
    buff->outPointer += rb_isFull(buff);                  //if buff is full, outPointer increases
    buff->values[buff->inPointer++ & (buff->size-1)] = val;
    buff->isEmpty = 0;
    pthread_mutex_unlock(&buff->mutex);
    return pthread_cond_signal(&buff->cond) ? 0 : 1;    //return 1 if ok
#endif
}

DataType rb_deque(RingBuffer *buff)
{
//    ValuesType ret;
    DataType ret;
#ifdef SAFEWRITE
    sem_wait(&buff->readsem);
    pthread_mutex_lock(&buff->mutex);
    ret = buff->values[(buff->outPointer++ & (buff->size-1))];
    pthread_mutex_unlock(&buff->mutex);
    sem_post(&buff->writesem);
#else
    pthread_mutex_lock(&buff->mutex);
    while(buff->isEmpty)
        pthread_cond_wait(&buff->cond, &buff->mutex);
    ret = buff->values[(buff->outPointer++ & (buff->size-1))];
    if((buff->outPointer & (buff->size-1)) == (buff->inPointer & (buff->size-1)))
        buff->isEmpty = 1;
    pthread_mutex_unlock(&buff->mutex);
#endif
    return ret;
}

int rb_remaining(RingBuffer *buff)
{
    int in, out;
    in = (buff->inPointer & (buff->size-1));
    out = (buff->outPointer & (buff->size-1));
    return in-out;
}

void rb_destroy(RingBuffer *buff)
{
    free(buff->values);
    free(buff);
}
