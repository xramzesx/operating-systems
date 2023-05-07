#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "common.h"
#include "linkedlist.h"

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

void unlink_semaphores();
void unlink_shared_memory();

void create_semaphores();
void create_shared_memory();

void close_semaphores();
void close_shared_memory();

//// SIGNALS ///

LinkedList * barbers_list;
LinkedList * clients_list;

//// MAIN ////

void handle_exit();
void handle_sigint();

int main (int argc, char ** argv) {

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

    //// SIGNALS ///

    barbers_list = init_linked_list();
    clients_list = init_linked_list();

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;

    sigaction(SIGINT, &sa, NULL);

    //// EXIT ///

    atexit(handle_exit);

    //// OPEN IPC ////

    printf("[initializing ipc]\n");

    unlink_semaphores();
    unlink_shared_memory();

    create_semaphores();
    create_shared_memory();

    printf("[start]\n\n");

    //// SPAWN BARBERS ////

    for (int i = 0; i < num_barbers; i++) {
        int pid = fork();
        
        switch (pid) {
            case -1:
                exit(2);
                break;
            case 0:
                execl(EXEC_BARBER, EXEC_BARBER, NULL);
                break;        
            default:
                list_append(barbers_list, pid);
                break;
        }

    }

    //// SPAWN CLIENTS ////

    for (int i = 0; i < num_clients || argc == 4; i++) {
        int pid = fork();    
        switch (pid) {
            case -1:
                exit(2);
                break;
            case 0:
                execl(EXEC_CLIENT, EXEC_CLIENT, NULL);
                break;        
            default:
                list_append(barbers_list, pid);
                break;
        }

        usleep(rand() % 500000 + 100000);
    }

    //// WAIT FOR CHILD PROCESSES ////

    while(wait(NULL) > 0);

    for (int i = 0; i < shared_queue->size; i++) {
        printf("[%d]: %d\n", i, shared_queue->queue[i]);
    }

    printf("\n[cleaning ipc]\n");

    //// CLOSE IPC ///

    close_semaphores();
    close_shared_memory();

    unlink_semaphores();
    unlink_shared_memory();

    printf("\n[finish]\n");

    return 0;
}

//// OPEN IPC BODY ////

void unlink_semaphores() {
    unlink_semaphore(SEM_NAME_BARBER);
    unlink_semaphore(SEM_NAME_QUEUE);
}

void unlink_shared_memory(){
    remove_shared_queue(SHARED_NAME_QUEUE);
    remove_shared_queue(SHARED_NAME_CHAIRS);
}

void create_semaphores() {
    sem_barber = create_semaphore(SEM_NAME_BARBER, 1, 0);
    sem_chair  = create_semaphore(SEM_NAME_CHAIR, 1, num_chairs );
    sem_queue = create_semaphore(SEM_NAME_QUEUE, 1, 0);
    post_semaphore(sem_queue);
}

void create_shared_memory() {
    //// SHARED MEMORY ////

    shared_queue = attach_shared_queue(SHARED_NAME_QUEUE);
    
    if (queue_is_error(shared_queue)) {
        exit(1);
    }
    shared_queue->size = 0;
    shared_queue->capacity = num_queue;

    //// SHARED CHAIRS ////

    shared_chairs = attach_shared_queue(SHARED_NAME_CHAIRS);

    if (queue_is_error(shared_chairs)) {
        exit(1);
    }

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

//// HANDLE EXIT ///

void handle_exit() {
    destroy_linked_list(barbers_list);
    destroy_linked_list(clients_list);
}

void handle_sigint(int code) {
    printf("\nTerminated\n");
    fflush(stdout);
    exit(0);
}