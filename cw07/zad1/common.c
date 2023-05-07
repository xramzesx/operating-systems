#include "common.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

Semaphore create_semaphore(const char * name, int sem_quantity, int initial ) {
    key_t key = ftok(PROJECT_PATHNAME, name[0]);
    
    if (key == -1) {
        perror("Failed to create semaphore key");
        return -1;
    }

    Semaphore smid = semget(key, 1, 0666 | IPC_CREAT);

    if ( smid == -1 ) {
        fprintf(stderr, "Failed to create semaphore '%s': ", name);
        perror("");
        return -1;
    }

    if (semctl(smid, 0, SETVAL, initial) == -1) {
        perror("Failed to set semaphore value");
        return -1;
    }

    return smid;
}

Semaphore open_semaphore(const char * name ) {
    key_t key = ftok(PROJECT_PATHNAME, name[0]);

    if ( key == -1 ) {
        perror("Failed to create semaphore key");
        return -1;
    }

    return semget(key, 1, 0);
}

void close_semaphore(Semaphore sm) {}

void unlink_semaphore(const char * name) {
    key_t key = ftok(PROJECT_PATHNAME, name[0]);
    
    if (key == -1) {
        perror("Failed to create semaphore key");
        return;
    }
    
    Semaphore smid = semget(key, 0, 0);

    if ( smid == -1 ) {
        fprintf(stderr, "Failed to get semaphore '%s': ", name);
        perror("");
        return;
    }

    if (semctl(smid, 0, IPC_RMID, 0) == -1) {
        fprintf(stderr, "Failed to remove semaphore '%s': ", name);
        perror("");
        return;
    }

    printf("Semaphore '%s' removed\n", name);
}

int get_semaphore_value(Semaphore sm) {
    return semctl(sm, 0, GETVAL, NULL);
}


void post_semaphore(Semaphore sm) {
    struct sembuf operations;

    operations.sem_num = 0;
    operations.sem_op = 1;
    operations.sem_flg = SEM_UNDO;

    if (semop(sm, &operations, 1) == -1) {
        fprintf(stderr, "Failed to post semaphore %d: ", sm);
        perror("");
    }
}

void wait_semaphore(Semaphore sm) {
    struct sembuf operations;

    operations.sem_num = 0;
    operations.sem_op = -1;
    operations.sem_flg = SEM_UNDO;

    if (semop(sm, &operations, 1) == -1) {
        fprintf(stderr, "Failed to wait semaphore %d: ", sm);
        perror("");
    }
}

//// QUEUE ///

static int get_shared_memory_id( const char * name, int size ) {
    key_t key = ftok(PROJECT_PATHNAME, name[0] + 10);
    return key == -1 ? -1 : shmget(key, size, 0666 | IPC_CREAT );
}

SharedStruct * attach_shared_queue( const char * name ){
    Semaphore shmid = get_shared_memory_id(name, SHARED_SIZE);
    
    if (shmid == -1) {
        fprintf(stderr, "No identifier for file : '%s'\n", name);
        return NULL;
    }
    
    SharedStruct * shared = shmat(shmid, NULL, 0);

    if ( shared == (SharedStruct *)(-1)) {
        fprintf(stderr, "Failed to load shared memory block with id %d\n", shmid);
        return NULL;
    }

    return shared;
}

bool detach_shared_queue( SharedStruct * shared ) {
    return (shmdt(shared) != -1);
}


//// QUEUE METHODS ////

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