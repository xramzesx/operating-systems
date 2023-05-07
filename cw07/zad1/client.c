#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define CLIENT_ID getpid()

#include "common.h"

int get_hairstyle() {
    return CLIENT_ID % 5 + 2;
}

int main (int argc, char ** argv) {

    SharedStruct * shared_queue  = attach_shared_queue(SHARED_NAME_QUEUE);
    SharedStruct * shared_chairs = attach_shared_queue(SHARED_NAME_CHAIRS);

    printf("+ [client] %d want %d hairstyle\n", CLIENT_ID, get_hairstyle());

    fflush(stdout);

    Semaphore sm = open_semaphore(SEM_NAME_QUEUE);
    /// lock shared queue
    wait_semaphore(sm);

    if (!queue_is_full(shared_chairs)) {

        queue_push(shared_chairs, get_hairstyle());

        post_semaphore(sm);
        return 0;
    }
    
    if (queue_is_full(shared_queue)){
        printf("- [client]: Queue is full, client %d escape from the barber\n", CLIENT_ID);
        post_semaphore(sm);
        return 0;
    }

    printf("  [client] %d wait in queue at %d. place\n", CLIENT_ID, shared_queue->size);

    queue_push(shared_queue, get_hairstyle());
        
    fflush(stdout);

    /// unlock shared queue
    post_semaphore(sm);

    detach_shared_queue(shared_queue);
    detach_shared_queue(shared_chairs);

    return 0;
}