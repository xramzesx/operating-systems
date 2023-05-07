#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#include "common.h"

//// EXEC ////

#define EXEC_BARBER "./barber.exe"
#define EXEC_CLIENT "./client.exe"

//// MAIN PARAMETERS ////

int num_barbers = 0;
int num_chairs  = 0;
int num_clients = 0;
int num_queue = 0;

//// IPC OBJECTS ////

Semaphore sem_barber;
Semaphore sem_chair;
Semaphore sem_queue;

SharedStruct * shared_queue;
SharedStruct * shared_chairs;

//// OPEN & CLOSE IPC HEADERS ////

void create_semaphores();
void create_shared_memory();

void close_semaphores();
void close_shared_memory();

//// MAIN ////

int main (int argc, char ** argv) {
    printf("[start]\n\n");

    srand(time(NULL));

    if ( argc < 4 ) {
        perror("Not enough arguments");
        return 1;
    }
    
    if (argc > 5) {
        perror("Too many arguments");
        return 2;        
    }

    //// PARSE INPUT ////

    num_barbers = atoi(argv[1]); // M
    num_chairs  = atoi(argv[2]); // N
    num_queue   = atoi(argv[3]); // P

    if (argc == 5)
        num_clients = atoi(argv[4]); // C

    //// OPEN IPC ////

    create_semaphores();
    create_shared_memory();

    //// SPAWN BARBERS ////

    for (int i = 0; i < num_barbers; i++) {
        if (fork() == 0) {
            execl(EXEC_BARBER, EXEC_BARBER, NULL);
        }
    }

    //// SPAWN CLIENTS ////

    for (int i = 0; i < num_clients || argc == 4; i++) {
        if (fork() == 0) {
            execl(EXEC_CLIENT, EXEC_CLIENT, NULL);
        }
        usleep(rand() % 500000 + 100000);
    }

    //// WAIT FOR CHILD PROCESSES ////

    while(wait(NULL) > 0);

    for (int i = 0; i < shared_queue->size; i++) {
        printf("[%d]: %d\n", i, shared_queue->queue[i]);
    }

    //// CLOSE IPC ///

    close_semaphores();
    close_shared_memory();

    printf("\n[finish]\n");

    return 0;
}

//// OPEN IPC BODY ////

void unlink_semaphores() {
    unlink_semaphore(SEM_NAME_BARBER);
    unlink_semaphore(SEM_NAME_QUEUE);
}

void create_semaphores() {
    sem_barber = create_semaphore(SEM_NAME_BARBER, 1, 0);
    sem_chair  = create_semaphore(SEM_NAME_CHAIR, 1, num_chairs );
    sem_queue = create_semaphore(SEM_NAME_QUEUE, 1, 0);
    post_semaphore(sem_queue);
}

void create_shared_memory() {
    shared_queue = attach_shared_queue(SHARED_NAME_QUEUE);
    shared_queue->size = 0;
    shared_queue->capacity = num_queue;

    shared_chairs = attach_shared_queue(SHARED_NAME_CHAIRS);
    shared_chairs->size = 0;
    shared_chairs->capacity = num_chairs;
}

//// CLOSE IPC BODY ////

void close_semaphores() {
    close_semaphore(sem_barber);
    close_semaphore(sem_queue);
}

void close_shared_memory() {
    detach_shared_queue(shared_queue);
    detach_shared_queue(shared_chairs);
}
