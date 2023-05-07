#include "common.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

Semaphore create_semaphore(const char * name, int sem_quantity, int initial ) {
    
    Semaphore sem = sem_open(name, O_CREAT | O_RDWR, 0644, initial);

    if (sem == SEM_FAILED) {
        fprintf(stderr, "Failed to create semaphore '%s': ", name);
        perror("");
        return NULL;
    }

    return sem;
}

Semaphore open_semaphore(const char * name ) {
    Semaphore sem = sem_open(name, 0);

    if (sem == SEM_FAILED) {
        fprintf(stderr, "Failed to open semaphore '%s': ", name);
        perror("");
        return NULL;
    }

    return sem;
}

void close_semaphore(Semaphore sm) {
    sem_close(sm);
}

void unlink_semaphore(const char * name) {
    sem_unlink(name);
}

int get_semaphore_value(Semaphore sm) {
    int result = -1;
    
    if (sem_getvalue(sm, &result) == -1) {
        perror("Failed to get semaphore value");
        return -1;
    }
    
    return result;
}

void post_semaphore(Semaphore sm) {
    sem_post(sm);
}

void wait_semaphore(Semaphore sm) {
    sem_wait(sm);
}

//// QUEUE ///


SharedStruct * attach_shared_queue( const char * name ){
    int shmid =  shm_open(name, O_CREAT | O_RDWR, 0644);
    
    if (shmid == -1) {
        fprintf(stderr, "No identifier for file : '%s' : ", name);
        perror("");
        return NULL;
    }
    
    if (ftruncate(shmid, SHARED_SIZE) == -1) {
        fprintf(stderr, "Failed to load shared memory block with id %d : ", shmid);
        perror("");
        return NULL;
    }
    
    SharedStruct * shared = mmap(
        NULL, 
        SHARED_SIZE, 
        PROT_READ | PROT_WRITE, 
        MAP_SHARED, 
        shmid, 
        0
    );

    if ( shared == (SharedStruct *) -1) {
        fprintf(stderr, "Failed to map shared memory block with id %d: ", shmid);
        perror("");
        return NULL;
    }

    return shared;
}

bool detach_shared_queue( SharedStruct * shared ) {
    return (munmap(shared, SHARED_SIZE) != -1);
}

bool remove_shared_queue( const char * name ) {
    return (shm_unlink(name) != -1);
}

//// QUEUE METHODS ////

bool queue_is_error(SharedStruct * shared) {
    return shared == NULL;
}

bool queue_is_full(SharedStruct * shared) {
    return shared->size == shared->capacity;
}

bool queue_is_empty(SharedStruct * shared) {
    return shared->size == 0;
}

void queue_push(SharedStruct * shared, int value) {
    if (queue_is_full(shared)) {
        return;
    }
    shared->queue[shared->size++] = value;
}

int queue_pop(SharedStruct * shared) {
    if (queue_is_empty(shared)) {
        return -1;
    }
    
    int result = shared->queue[0];

    for (int i = 1; i < shared->size; i++) {
        shared->queue[i - 1] = shared->queue[i];
    }

    shared->size--;
    shared->queue[shared->size] = 0;

    return result;
}