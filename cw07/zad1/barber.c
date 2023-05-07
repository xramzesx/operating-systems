#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdbool.h>

#include "common.h"

#define BARBER_ID getpid()

Semaphore sem_barber;
Semaphore sem_chairs;
Semaphore sem_queue;

SharedStruct * shared_queue;
SharedStruct * shared_chairs;

int get_hairstyle();

int main (int argc, char ** argv) {
    sem_barber = open_semaphore(SEM_NAME_BARBER);
    sem_chairs = open_semaphore(SEM_NAME_CHAIR);
    sem_queue  = open_semaphore(SEM_NAME_QUEUE);

    shared_queue = attach_shared_queue(SHARED_NAME_QUEUE);
    shared_chairs = attach_shared_queue(SHARED_NAME_CHAIRS);

    post_semaphore(sem_barber);
    printf( 
        "+ [barber] with id %d join the game as: %d \n",
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

        printf("> [barber] %d made hairstyle no. %d\n", getpid(), hairstyle);
        fflush(stdout);

        /// Free chair
        post_semaphore(sem_chairs);
        
        attempts = 2;
        usleep(500000);
    }
    
    detach_shared_queue(shared_queue);
    detach_shared_queue(shared_chairs);

    printf("- [barber] with id %d already left \n", getpid());

    return 0;
}

int get_hairstyle() {
    wait_semaphore(sem_queue);

    while (!queue_is_empty(shared_queue) && !queue_is_full(shared_chairs)) {
        queue_push(
            shared_chairs, 
            queue_pop(shared_queue)
        );
    }

    /// get hairstyle
    int hairstyle = queue_pop(shared_chairs);
    
    /// Unlock shared memory
    post_semaphore(sem_queue);
    
    return hairstyle;
}