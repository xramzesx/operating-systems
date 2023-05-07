#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"

int get_hairstyle() {
    return getpid() % 5 + 2;
}

int main (int argc, char ** argv) {

    SharedStruct * shared = attach_shared_queue(SHARED_NAME_QUEUE);
    fflush(stdout);

    Semaphore sm = open_semaphore(SEM_NAME_QUEUE);
    /// lock shared queue
    wait_semaphore(sm);

    if (queue_is_full(shared)){
        printf("[client]: Queue is full, client %d escape from the barber\n", getpid());
        post_semaphore(sm);
        return 0;
    }
    printf("[client] id: %d, hairstyle: %d, queue: %d\n", getpid(), get_hairstyle() ,shared->size);

    queue_push(shared, get_hairstyle());
    
    printf("[client] id: %d is %d\n",getpid(), shared->size);
    
    fflush(stdout);

    /// unlock shared queue
    post_semaphore(sm);

    detach_shared_queue(shared);

    return 0;
}