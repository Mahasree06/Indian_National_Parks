#include <stdio.h>

#include "../include/queue.h"


void queue_init(SharedQueue *queue)
{
    queue->front = 0;
    queue->rear = 0;

    pthread_mutex_init(
        &queue->mutex,
        NULL
    );

    sem_init(
        &queue->empty,
        0,
        QUEUE_SIZE
    );

    sem_init(
        &queue->full,
        0,
        0
    );
}


void queue_destroy(SharedQueue *queue)
{
    pthread_mutex_destroy(
        &queue->mutex
    );

    sem_destroy(
        &queue->empty
    );

    sem_destroy(
        &queue->full
    );
}


void queue_push(
    SharedQueue *queue,
    ParkRecord record
)
{
    sem_wait(
        &queue->empty
    );

    pthread_mutex_lock(
        &queue->mutex
    );


    queue->records[
        queue->rear
    ] = record;


    queue->rear =
        (queue->rear + 1) % QUEUE_SIZE;


    pthread_mutex_unlock(
        &queue->mutex
    );


    sem_post(
        &queue->full
    );
}


ParkRecord queue_pop(
    SharedQueue *queue
)
{
    ParkRecord record;


    sem_wait(
        &queue->full
    );


    pthread_mutex_lock(
        &queue->mutex
    );


    record =
        queue->records[
            queue->front
        ];


    queue->front =
        (queue->front + 1) % QUEUE_SIZE;


    pthread_mutex_unlock(
        &queue->mutex
    );


    sem_post(
        &queue->empty
    );


    return record;
}
