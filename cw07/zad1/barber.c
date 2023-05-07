#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdbool.h>

#include "common.h"

Semaphore sem_barber;
Semaphore sem_chairs;
Semaphore sem_queue;

SharedStruct * shared;

int get_hairstyle();

int main (int argc, char ** argv) {
    sem_barber = open_semaphore(SEM_NAME_BARBER);
    sem_chairs = open_semaphore(SEM_NAME_CHAIR);
    sem_queue  = open_semaphore(SEM_NAME_QUEUE);

    shared = attach_shared_queue(SHARED_NAME_QUEUE);

    post_semaphore(sem_barber);
    printf( 
        "[barber] with id %d join the game as: %d \n",
        getpid(), 
        get_semaphore_value(sem_barber) 
    );

    fflush(stdout);

    // sleep(1);

    int attempts = 2;

    while (attempts) {
        /// lock chair & shared memory
        wait_semaphore(sem_chairs);

        int hairstyle = get_hairstyle();

        if (hairstyle != -1) {
            sleep(hairstyle);
        } else {
            attempts--;
            sleep(1);
            post_semaphore(sem_chairs);
            continue;
        }

        printf("Barber %d made hairstyle no. %d %d\n", getpid(), hairstyle, shared->size);
        fflush(stdout);

        /// Free chair
        post_semaphore(sem_chairs);
        
        attempts = 2;
        usleep(500000);
    }
    
    detach_shared_queue(shared);

    printf("[barber] with id %d already left \n", getpid());

    return 0;
}

int get_hairstyle() {
    wait_semaphore(sem_queue);

    /// get hairstyle
    int hairstyle = queue_pop(shared);
    
    /// Unlock shared memory
    post_semaphore(sem_queue);
    
    return hairstyle;
}