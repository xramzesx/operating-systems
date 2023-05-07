#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>

//// QUEUE CONFIG ////

#define PROJECT_PATHNAME getenv("HOME")

//// SEMAPHORE FILENAMES ////

#define SEM_NAME_BARBER "/sem_barber" // available barbers
#define SEM_NAME_QUEUE  "/sem_queue" // shared memory mutex control
#define SEM_NAME_CHAIR  "/sem_chair" // available chairs

//// SHARED MEMORY CONSTANTS ////

#define SHARED_MAX_QUEUE_SIZE 128
#define SHARED_NAME_QUEUE     "/shared_queue"
#define SHARED_NAME_CHAIRS    "/shared_chairs"

typedef struct {
    int capacity;
    int size;
    int queue[SHARED_MAX_QUEUE_SIZE];
} SharedStruct;

#define SHARED_SIZE sizeof(SharedStruct)

//// TYPEDEFS ////

typedef sem_t * Semaphore;

//// SEMAPHORES ////

Semaphore create_semaphore(const char * name, int sem_quantity, int initial);
Semaphore open_semaphore(const char * name);

void unlink_semaphore(const char * name);
void close_semaphore(Semaphore sm);

int get_semaphore_value(Semaphore sm);

void post_semaphore(Semaphore sm);
void wait_semaphore(Semaphore sm);

//// SHARED MEMORY ////

SharedStruct * attach_shared_queue( const char * name );
bool           detach_shared_queue( SharedStruct * queue );
bool           remove_shared_queue( const char * name );

bool queue_is_error(SharedStruct * shared);
bool queue_is_full(SharedStruct * shared);
bool queue_is_empty(SharedStruct * shared);

void queue_push(SharedStruct * shared, int value);
int  queue_pop(SharedStruct * shared);