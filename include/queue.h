#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <semaphore.h>

#define QUEUE_SIZE 10

typedef struct
{
    char park_name[256];
    char state_ut[100];
    char location[256];
    char formed_year[20];
    char notable_features[1000];
    char flora_fauna[1000];
    char rivers_lakes[1000];

} ParkRecord;


typedef struct
{
    ParkRecord records[QUEUE_SIZE];

    int front;
    int rear;

    pthread_mutex_t mutex;

    sem_t empty;
    sem_t full;

} SharedQueue;


void queue_init(SharedQueue *queue);

void queue_destroy(SharedQueue *queue);

void queue_push(
    SharedQueue *queue,
    ParkRecord record
);

ParkRecord queue_pop(
    SharedQueue *queue
);


#endif
